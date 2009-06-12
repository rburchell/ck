#include <contextproperty.h>

#include "listener.h"

#include <QCoreApplication>


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    Listener listener;

    return app.exec();
}
