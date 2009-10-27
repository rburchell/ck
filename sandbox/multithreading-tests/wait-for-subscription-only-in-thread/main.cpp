#include <contextproperty.h>

#include "thread.h"

#include <QCoreApplication>
#include <QThread>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    qDebug() << "MAIN THREAD:" << QCoreApplication::instance()->thread();

    // Start a thread which will create the ContextProperty.
    Thread thread;
    thread.start();

    qDebug() << "Entering main loop";
    return app.exec();
}
