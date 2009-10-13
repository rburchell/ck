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

            foreach(ContextProviderInfo prov, info->listProviders())
                out << "Proivder: " << prov.plugin << " (" << prov.constructionString << ")" << endl;

            out << "Documentation: " << info->doc() << endl;
            out << "Type: " << info->type() << endl;
        }
        out << "----------" << endl;
    }

    return 0;
}
