#include "propertyproxy.h"
#include "sconnect.h"
#include "contextproperty.h"
#include "property.h"
#include "logging.h"

PropertyProxy::PropertyProxy(QString key, bool enabled, QObject *parent) :
    QObject(parent), enabled(enabled)
{
    subscriber = new ContextProperty(key, this);
    provider = new ContextProvider::Property(key, this);
    sconnect(subscriber, SIGNAL(valueChanged()),
             this, SLOT(onValueChanged()));
    enable(enabled);
}

void PropertyProxy::enable(bool enable)
{
    qDebug() << (const char *)(enable ? "enabled" : "disabled") <<
        "proxying" << subscriber->key();
    enabled = enable;
    if (enable)
        provider->setValue(value);
}

QVariant PropertyProxy::realValue() const
{
    return value;
}

void PropertyProxy::onValueChanged()
{
    value = subscriber->value();
    if (enabled)
        provider->setValue(value);
}
