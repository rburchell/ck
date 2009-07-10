#ifndef THREAD_H
#define THREAD_H

#include <contextproperty.h>
#include <contextpropertyinfo.h>

#include <QThread>
#include <QDebug>

class Listener : public QObject
{
    Q_OBJECT

public:
    Listener()
        {
            cp = new ContextProperty("test.int");
            qDebug() << "Provider is: " << cp->info()->providerDBusName();
        }

    ContextProperty* cp;
};

class Thread : public QThread
{
    Q_OBJECT

protected:
    void run()
        {
            qDebug() << "Thread::run(), and current thread is" << QThread::currentThread();
            Listener listener;

            qDebug() << "Returning from run";
        }

};

#endif
