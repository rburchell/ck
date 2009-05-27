#include "propertylistener.h"
#include "contextproperty.h"
#include "sconnect.h"
#include <QDebug>
#include <QVariant>

PropertyListener::PropertyListener(ContextProperty *prop) :
    QObject(prop), prop(prop)
{
    sconnect(prop, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
    qDebug() << prop->key() << " subscribtion started";
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
