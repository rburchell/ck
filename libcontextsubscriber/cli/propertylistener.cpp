#include "propertylistener.h"
#include "contextproperty.h"
#include "sconnect.h"
#include <QDebug>

PropertyListener::PropertyListener(ContextProperty *prop, QObject *parent) :
    QObject(parent), prop(prop)
{
    // TODO: when the library is asynchronous, this will have to be changed
    sconnect(prop, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
    QTextStream out(stdout);
    qDebug() << prop->key() << "has been subscribed";
}

void PropertyListener::onValueChanged()
{
    QTextStream out(stdout);
    out << prop->key();
    if (prop->value().isNull()) {
        out << " is Unknown";
    } else {
        out << " = " << prop->value().typeName() << ":" << prop->value().toString();
    }
    out << endl;
}
