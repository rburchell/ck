#ifndef THREAD_H
#define THREAD_H

#include <contextproperty.h>

#include <QThread>
#include <QDebug>

class Listener : public QObject
{
    Q_OBJECT

public:
    Listener()
        {
            cp = new ContextProperty("test.int");
            connect(cp, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
            qDebug() << "**** Starting to wait";
            cp->waitForSubscription();
            qDebug() << "**** Waiting is done";
            qDebug() << "After waiting, the value is" << cp->value();
            sleep(1);
            qDebug() << "After waiting 1 s more, the value is" << cp->value();

        }

    ContextProperty* cp;

public slots:
    void onValueChanged()
        {
            qDebug() << "Listener::valueChanged(), and current thread is" << QThread::currentThread();
            qDebug() << "The value is:" << cp->value();
//            exit(1);
        }
};

class Thread : public QThread
{
    Q_OBJECT

protected:
    void run()
        {
            qDebug() << "SUB  THREAD:" << QThread::currentThread();
            Listener listener;
            exec();
        }

};

#endif
