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

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <QtGui/qpa/qplatforminputcontextplugin_p.h>

typedef struct _GDBusProxy GDBusProxy;

namespace ubuntu
{

class ProxyPlugin : public QPlatformInputContextPlugin
{
private:

    Q_OBJECT

    Q_PLUGIN_METADATA(IID QPlatformInputContextFactoryInterface_iid FILE "plugin.json")

public:

    explicit ProxyPlugin(QPlatformInputContextPlugin *parent = 0);
    virtual ~ProxyPlugin();

    virtual QPlatformInputContext *create(const QString &key, const QStringList &paramList);

    virtual QPlatformInputContext *createSlave() const;

Q_SIGNALS:

    void frameworkChanged(const ProxyPlugin &plugin) const;

private:

    GDBusProxy *proxy;
};

} /* namespace ubuntu */

#endif /* __PLUGIN_H__ */
