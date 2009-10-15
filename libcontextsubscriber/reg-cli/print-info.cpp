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
        bool declared = info->declared();
        out << "Existence: " << (declared ? "true" : "false")  << endl;
        if (declared) {

            // This will be obsoleted by the upcoming context-ls, so,
            // don't change the old format and just print out the
            // first provider.
            ContextProviderInfo prov = info->providers()[0];
            out << "Provider DBus type: " << prov.constructionString.split(":")[0] << endl;
            out << "Provider DBus name: " << prov.constructionString.split(":")[1] << endl;
            out << "Documentation: " << info->doc() << endl;
        }
        out << "----------" << endl;
    }

    return 0;
}
