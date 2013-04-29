#include "propertyproxy.h"
#include "sconnect.h"
#include "contextproperty.h"
#include "contextpropertyinfo.h"
#include "property.h"
#include "logging.h"

PropertyProxy::PropertyProxy(QString key, bool enabled, QObject *parent) :
    QObject(parent), enabled(enabled)
{
    provider = new ContextProvider::Property(key, this);
    subscriber = new ContextProperty(key, this);
    subscriber->unsubscribe();
    sconnect(provider, SIGNAL(firstSubscriberAppeared(QString)),
             this, SLOT(onFirstSubscriber(QString)));
    sconnect(provider, SIGNAL(lastSubscriberDisappeared(QString)),
             this, SLOT(onLastSubscriber()));
    sconnect(subscriber, SIGNAL(valueChanged()),
             this, SLOT(onValueChanged()));
    enable(enabled);
}

void PropertyProxy::enable(bool enable)
{
    qDebug() << (const char *)(enable ? "enabled" : "disabled") <<
        "proxying" << provider->key();
    enabled = enable;
    if (enable)
        provider->setValue(value);
}

QVariant PropertyProxy::realValue() const
{
    return value;
}

QString PropertyProxy::type() const
{
    return subscriber->info()->type();
}

void PropertyProxy::onFirstSubscriber(const QString &key)
{
    contextDebug() << "First client subscription received, commander subscribes to provider" << subscriber->key();
    subscriber->subscribe();
}

void PropertyProxy::onLastSubscriber()
{
    contextDebug() << "Last client unsubscribed, commander unsubscribes from provider" << subscriber->key();
    subscriber->unsubscribe();
}

void PropertyProxy::onValueChanged()
{
    value = subscriber->value();
    if (enabled)
        provider->setValue(value);
    QTextStream out(stdout);
    out << "real: " << type() << " " << subscriber->key();
    if (realValue().isNull())
        out << " is Unknown" << endl;
    else
        out << " = " << realValue().toString() << endl;
    out.flush();
}
