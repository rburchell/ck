#include "contextproperty.h"
#include "propertylistener.h"
#include "commandwatcher.h"
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (getenv("CONTEXT_CLI_DISABLE_TYPE_CHECK"))
        ContextProperty::setTypeCheck(false);
    else
        ContextProperty::setTypeCheck(true);

    if (getenv("CONTEXT_CLI_IGNORE_COMMANDER"))
        ContextProperty::ignoreCommander();

    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    if (args.count() <= 1)
        qWarning() << "Started without properties, if you want properties at startup, pass them as arguments";

    QMap<QString, ContextProperty*> properties;

    args.pop_front();
    foreach (QString key, args) {
        if (properties[key] == 0) {
            properties[key] = new ContextProperty(key, QCoreApplication::instance());
            new PropertyListener(properties[key]);
        }
    }

    new CommandWatcher(STDIN_FILENO, &properties, QCoreApplication::instance());

    return app.exec();
}
