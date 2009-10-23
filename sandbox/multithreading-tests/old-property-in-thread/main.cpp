#include <contextproperty.h>

#include "thread.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    qDebug() << "Main thread is" << QThread::currentThread();

    ContextProperty* cp = new ContextProperty("test.int");
    delete cp;
    // Creating a ContextProperty will result in creating a PropertyHandle,
    // and the PropertyHandle is not deleted ever.

    // Start a thread which will create the same ContextProperty.
    Thread thread;
    thread.start();

    return app.exec();
}

