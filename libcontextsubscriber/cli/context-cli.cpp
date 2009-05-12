#include "contextproperty.h"
#include "propertylistener.h"
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QDebug>

int main(int argc, char **argv)
{
    setenv("CONTEXT_PROVIDERS", "./context-providers.cdb", 0);

    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    if (args.count() <= 1) {
        QTextStream out(stdout);
        out << "Usage: " << app.arguments().at(0) << " <properties...>" << endl;
        return 0;
    }

    args.pop_front();
    foreach (QString key, args) {
        new PropertyListener(new ContextProperty(key, &app), &app);
    }

    return app.exec();
}

