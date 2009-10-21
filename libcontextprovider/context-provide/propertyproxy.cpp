#include "propertyproxy.h"
#include "sconnect.h"
#include "contextproperty.h"
#include "property.h"
#include "logging.h"

PropertyProxy::PropertyProxy(QString key, QObject *parent) :
    QObject(parent)
{
    ContextProperty::ignoreCommander();
    subscriber = new ContextProperty(key, this);
    provider = new ContextProvider::Property(key, this);
    sconnect(subscriber, SIGNAL(valueChanged()),
             this, SLOT(onValueChanged()));
    contextDebug() << "Started proxying " << key;
}

void PropertyProxy::onValueChanged()
{
    provider->setValue(subscriber->value());
}
