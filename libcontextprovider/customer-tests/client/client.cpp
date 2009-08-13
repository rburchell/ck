#include "commandwatcher.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    new CommandWatcher(STDIN_FILENO, QCoreApplication::instance());

    return app.exec();
}
