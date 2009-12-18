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
        }

    ContextProperty* cp;

public Q_SLOTS:
    void onValueChanged()
        {
            qDebug() << "Listener::valueChanged(), and current thread is" << QThread::currentThread();
            qDebug() << "The value is:" << cp->value();
            exit(1);
        }
};

class Thread : public QThread
{
    Q_OBJECT

protected:
    void run()
        {
            qDebug() << "Thread::run(), and current thread is" << QThread::currentThread();
            Listener listener;
            exec();
        }

};

#endif
