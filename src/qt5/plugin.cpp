#include "plugin.h"

#include <QPluginLoader>
#include <gio/gio.h>

#include "context.h"

/* XXX */
#include <QDBusConnection>

namespace ubuntu
{

static void properties_changed_cb(GDBusProxy *proxy,
                                  GVariant   *changed_properties,
                                  GStrv       invalidated_properties,
                                  gpointer    user_data)
{
    ProxyPlugin *plugin = static_cast<ProxyPlugin *>(user_data);

    /* XXX */
    QDBusConnection::disconnectFromBus("QIBusProxy");

    Q_EMIT plugin->frameworkChanged(*plugin);
}

ProxyPlugin::ProxyPlugin(QPlatformInputContextPlugin *parent) : QPlatformInputContextPlugin(parent)
{
    proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
                                          G_DBUS_PROXY_FLAGS_GET_INVALIDATED_PROPERTIES,
                                          NULL,
                                          "com.ubuntu.InputMethodSwitcher",
                                          "/",
                                          "com.ubuntu.InputMethodSwitcher",
                                          NULL,
                                          NULL);

    g_signal_connect(proxy, "g-properties-changed", G_CALLBACK(properties_changed_cb), this);
}

ProxyPlugin::~ProxyPlugin()
{
    g_object_unref(proxy);
}

QPlatformInputContext *ProxyPlugin::create(const QString &key, const QStringList &paramList)
{
    ProxyContext *context = new ProxyContext;

    connect(this, SIGNAL(frameworkChanged(const ProxyPlugin &)), context, SLOT(slaveChanged(const ProxyPlugin &)));
    context->slaveChanged(*this);

    return context;
}

QPlatformInputContext *ProxyPlugin::createSlave() const
{
    GVariant *variant = g_dbus_proxy_get_cached_property(proxy, "Framework");
    QString framework(g_variant_get_string(variant, NULL));
    g_variant_unref(variant);

    if (framework.isEmpty())
      return NULL;

    QPluginLoader loader(QString(PLUGIN_DIR "/lib%1platforminputcontextplugin.so").arg(framework));
    QPlatformInputContextPlugin *plugin = qobject_cast<QPlatformInputContextPlugin *>(loader.instance());

    return plugin->create(framework, QStringList(framework));
}

} /* namespace ubuntu */
