#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QDebug>
#include "myobject.h"

class MyThread : public QThread
{
    Q_OBJECT

private:
    MyObject *obj;

public:
    MyThread(MyObject *obj) : obj(obj)
        {
        }

protected:
    void run()
        {
            qDebug() << QThread::currentThread();
            obj->queueOnce("five");
            obj->queueOnce("six");
            obj->queueOnce("five");
            obj->queueOnce("five");
            obj->queueOnce("six");
            obj->queueOnce("seven");
        }
};
#endif
