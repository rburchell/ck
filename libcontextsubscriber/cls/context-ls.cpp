#include <contextregistryinfo.h>
#include <contextpropertyinfo.h>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int retValue = 1;
    {
        QCoreApplication app(argc, argv);

        QStringList args = app.arguments();
        QString backendName = "";
        bool doc = false;
        bool provided = false;
        bool longListing = false;
        bool hasFilter = false;
        QString filter = "*";

        args.pop_front();
        foreach (QString arg, args) {
            if (arg == "--xml" || arg == "-x") {
                backendName = "xml";
            } else if (arg == "--cdb" || arg == "-c") {
                backendName = "cdb";
            } else if (arg == "--provided" || arg == "-p") {
                provided = true;
            } else if (arg == "--long" || arg == "-l") {
                longListing = true;
            } else if (arg == "--doc" || arg == "-d") {
                doc = true;
            } else {
                if (hasFilter) {
                    qWarning("WARNING: Only the first filter string is considered.");
                } else {
                    hasFilter = true;
                    filter = arg;
                }
            }
        }

        ContextRegistryInfo *regInfo = ContextRegistryInfo::instance(backendName);
        QStringList keys = regInfo->listKeys();
     
        QRegExp rx(filter, Qt::CaseSensitive, QRegExp::Wildcard);
        QTextStream out(stdout);
        foreach (QString key, keys) {
            if (!rx.exactMatch(key))
                continue;
            ContextPropertyInfo info(key);
            if (provided && !info.provided())
                continue;
            if (longListing) {
                for (int i = 0; i < info.providers().size(); i++ )
                    out << key << "\t" << info.type() << "\t" << info.providers()[i].plugin << "\t"
                        << info.providers()[i].constructionString << "\n";
            } else
                out << key << "\n";
            if (doc) {
                out << "Documentation: " << info.doc() << "\n";
            }
            retValue = 0;
        }
        out.flush();

    }
    return retValue;
}
