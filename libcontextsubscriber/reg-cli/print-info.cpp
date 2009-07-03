#include "contextproperty.h"
#include "contextpropertyinfo.h"
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    if (args.count() <= 1)
        qWarning() << "Usage: print-info PROPERTY...";

    args.pop_front();
    QTextStream out(stdout);

    foreach (QString key, args) {
        ContextProperty property(key);
        const ContextPropertyInfo* info = property.info();
        if (info == 0) {
            qWarning() << "Info for property" << key << "not available";
            return 1;
        }
        out << "Key: " << key << endl;
        out << "Existence: " << (info->exists() ? "true" : "false")  << endl;
        QDBusConnection::BusType busType = info->providerDBusType();
        out << "Provider DBus type: " << (busType == QDBusConnection::SessionBus ? "session" : "system") << endl;
        out << "Provider DBus name: " << info->providerDBusName() << endl;
        out << "Documentation: " << info->doc() << endl;
    }

    return 0;
}
