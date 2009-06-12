#include <contextproperty.h>

#include "thread.h"

#include <QCoreApplication>


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    // Start a thread which will create the ContextProperty.
    Thread thread;
    thread.start();

    return app.exec();
}

