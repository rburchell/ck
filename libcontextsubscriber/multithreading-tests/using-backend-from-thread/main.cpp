#include <contextproperty.h>

#include "thread.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    qDebug() << "Main thread is" << QThread::currentThread();

    // Start a thread which will use the backend
    Thread* thread = new Thread();
    thread->start();

    thread->wait();
    // Kill the thread
    qDebug() << "Thread stopped, deleting it";
    delete thread;
    qDebug() << "Entering main event loop";

    return app.exec();
}
