#include "propertyproxy.h"
#include "sconnect.h"
#include "contextproperty.h"
#include "property.h"
#include "logging.h"

PropertyProxy::PropertyProxy(QString key, QObject *parent) :
    QObject(parent), enabled(true)
{
    ContextProperty::ignoreCommander();
    subscriber = new ContextProperty(key, this);
    provider = new ContextProvider::Property(key, this);
    sconnect(subscriber, SIGNAL(valueChanged()),
             this, SLOT(onValueChanged()));
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
