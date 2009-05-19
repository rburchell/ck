#include "contextproperty.h"
#include "propertylistener.h"
#include "sconnect.h"
#include <QSocketNotifier>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QDebug>

int main(int argc, char **argv)
{
    setenv("CONTEXT_PROVIDERS", ".", 0);
    ContextProperty::setTypeCheck(true);

    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    if (args.count() <= 1) {
        QTextStream out(stdout);
        out << "Usage: " << app.arguments().at(0) << " <properties...>" << endl;
        return 0;
    }

    args.pop_front();
    foreach (QString key, args) {
        ContextProperty* prop = new ContextProperty(key);
        prop->setParent(new PropertyListener(prop, &app));
    }

    QSocketNotifier stdinWatcher(0, QSocketNotifier::Read, &app);
    sconnect(&stdinWatcher,
             SIGNAL(activated(int)),
             &app,
             SLOT(quit()));

    return app.exec();
}

