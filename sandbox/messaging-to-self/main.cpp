#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include "myobject.h"
#include "mythread.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    qDebug() << "main thread:" << QThread::currentThread();

    MyObject a;
    MyThread t(&a);
    t.start();

//    sleep(1);

    return app.exec();
}
