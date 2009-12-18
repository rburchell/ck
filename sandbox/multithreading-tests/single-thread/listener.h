#ifndef LISTENER_H
#define LISTENER_H

#include <contextproperty.h>

#include <QDebug>

class Listener : public QObject
{
    Q_OBJECT

public:
    Listener()
        {
            cp = new ContextProperty("test.int");
            connect(cp, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
        }

    ContextProperty* cp;

public Q_SLOTS:
    void onValueChanged()
        {
            qDebug() << "Listener::valueChanged()";
            qDebug() << "The value is:" << cp->value();
            exit(1);
        }
};

#endif
