#include "contextproperty.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    ContextProperty prop("Context.Test.Whatever");

    qDebug() << "I'm here" << prop.key();

    return app.exec();
}

