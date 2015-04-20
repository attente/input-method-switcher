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
