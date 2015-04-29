/*
 * Copyright (C) 2015 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: William Hua <william.hua@canonical.com>
 */

#include <signal.h>
#include <stdio.h>
#include <gio/gio.h>

#include "xml.h"

static GMainLoop *main_loop;
static gchar *framework;
static guint object_id;

static void
sa_handler_cb (int signum)
{
  if (main_loop)
    g_main_loop_quit (main_loop);
}

static GVariant *
get_property_cb (GDBusConnection  *connection,
                 const gchar      *sender,
                 const gchar      *object_path,
                 const gchar      *interface_name,
                 const gchar      *property_name,
                 GError          **error,
                 gpointer          user_data)
{
  return g_variant_new_string (framework ? framework : "");
}

static void
bus_acquired_cb (GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
  GDBusInterfaceVTable vtable = { 0 };
  GDBusNodeInfo *info;
  GError *error = NULL;

  vtable.get_property = get_property_cb;

  info = g_dbus_node_info_new_for_xml (INTROSPECTION_XML, &error);

  if (!info)
    g_error ("could not parse introspection xml: %s", error->message);

  object_id = g_dbus_connection_register_object (connection,
                                                 "/",
                                                 info->interfaces[0],
                                                 &vtable,
                                                 NULL,
                                                 NULL,
                                                 &error);

  if (!object_id)
    {
      g_warning ("could not register object: %s", error->message);
      g_clear_error (&error);

      if (main_loop)
        g_main_loop_quit (main_loop);
    }

  g_dbus_node_info_unref (info);
}

static void
bus_name_lost_cb (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
  if (main_loop)
    g_main_loop_quit (main_loop);
}

static void
start_framework (const gchar *framework)
{
  if (framework && framework[0])
    {
      gchar *path = g_strdup_printf (SCRIPT_DIR "/%s", framework);

      if (g_file_test (path, G_FILE_TEST_EXISTS))
        {
          gchar *command = g_strdup_printf ("sh " SCRIPT_DIR "/%s start", framework);

          g_spawn_command_line_sync (command, NULL, NULL, NULL, NULL);

          g_free (command);
        }

      g_free (path);
    }
}

static void
stop_framework (const gchar *framework)
{
  if (framework && framework[0])
    {
      gchar *path = g_strdup_printf (SCRIPT_DIR "/%s", framework);

      if (g_file_test (path, G_FILE_TEST_EXISTS))
        {
          gchar *command = g_strdup_printf ("sh " SCRIPT_DIR "/%s stop", framework);

          g_spawn_command_line_sync (command, NULL, NULL, NULL, NULL);

          g_free (command);
        }

      g_free (path);
    }
}

static void
changed_framework_cb (GSettings *settings,
                      gchar     *key,
                      gpointer   user_data)
{
  gchar *old_framework;
  gchar *new_framework;

  old_framework = framework;
  new_framework = g_settings_get_string (settings, key);

  if (g_strcmp0 (new_framework, old_framework))
    {
      GError *error = NULL;
      GDBusConnection *connection = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, &error);
      GVariant *parameters;

      if (connection)
        {
          framework = NULL;

          parameters = g_variant_new_parsed ("('com.ubuntu.InputMethodSwitcher', { 'Framework' : <''> }, @as [])");
          g_dbus_connection_emit_signal (connection,
                                         NULL,
                                         "/",
                                         "org.freedesktop.DBus.Properties",
                                         "PropertiesChanged",
                                         parameters,
                                         NULL);

          stop_framework (old_framework);
          start_framework (new_framework);

          framework = new_framework;

          parameters = g_variant_new_parsed ("('com.ubuntu.InputMethodSwitcher', { 'Framework' : <%s> }, @as [])", new_framework);
          g_dbus_connection_emit_signal (connection,
                                         NULL,
                                         "/",
                                         "org.freedesktop.DBus.Properties",
                                         "PropertiesChanged",
                                         parameters,
                                         NULL);

          g_object_unref (connection);
        }
      else
        {
          g_warning ("could not connect to session bus: %s", error->message);
          g_clear_error (&error);
        }
    }
  else
    g_free (new_framework);
}

int
main (int   argc,
      char *argv[])
{
  struct sigaction new_sigaction = { 0 };
  struct sigaction old_sigaction_sigint;
  struct sigaction old_sigaction_sigquit;
  struct sigaction old_sigaction_sigterm;

  GSettings *settings;
  guint name_id;

  new_sigaction.sa_handler = sa_handler_cb;

  if (sigaction (SIGINT,  &new_sigaction, &old_sigaction_sigint)  ||
      sigaction (SIGQUIT, &new_sigaction, &old_sigaction_sigquit) ||
      sigaction (SIGTERM, &new_sigaction, &old_sigaction_sigterm))
    perror ("could not install signal handlers");

  settings = g_settings_new ("com.ubuntu.input-method-switcher");
  g_signal_connect (settings, "changed::framework", G_CALLBACK (changed_framework_cb), NULL);
  framework = g_settings_get_string (settings, "framework");

  name_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                            "com.ubuntu.InputMethodSwitcher",
                            G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT,
                            bus_acquired_cb,
                            NULL,
                            bus_name_lost_cb,
                            NULL,
                            NULL);

  start_framework (framework);

  main_loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (main_loop);
  g_clear_pointer (&main_loop, g_main_loop_unref);

  stop_framework (framework);

  if (object_id)
    {
      GError *error = NULL;
      GDBusConnection *connection = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, &error);

      if (connection)
        {
          g_dbus_connection_unregister_object (connection, object_id);
          g_object_unref (connection);
        }
      else
        {
          g_warning ("could not connect to session bus: %s", error->message);
          g_clear_error (&error);
        }
    }

  g_bus_unown_name (name_id);

  g_clear_pointer (&framework, g_free);
  g_clear_object (&settings);

  if (sigaction (SIGTERM, &old_sigaction_sigterm, NULL) ||
      sigaction (SIGQUIT, &old_sigaction_sigquit, NULL) ||
      sigaction (SIGINT,  &old_sigaction_sigint,  NULL))
    perror ("could not restore signal handlers");

  return 0;
}
