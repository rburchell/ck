#ifndef MYOBJECT_H
#define MYOBJECT_H

#include "queuedinvoker.h"
#include <QDebug>
#include <QThread>

class MyObject : public QueuedInvoker
{
    Q_OBJECT

private:
    int x;

public:
    MyObject() : x(0)
        {
        }

    Q_INVOKABLE void five()
        {
            qDebug() << "MyObject::five" << QThread::currentThread() << ++x;
        }
    Q_INVOKABLE void six()
        {
            qDebug() << "MyObject::six" << QThread::currentThread() << ++x;
        }
    Q_INVOKABLE void seven()
        {
            qDebug() << "MyObject::seven" << QThread::currentThread() << ++x;
        }
};
#endif


