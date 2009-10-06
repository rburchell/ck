#include <infobackend.h>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>
#include <stdlib.h>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    QString backendName = "";
    bool provided = false;
    bool longListing = false;
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
        } else {
            filter = arg;
        }
    }

    InfoBackend *backend = InfoBackend::instance(backendName);
    QStringList keys = backend->listKeys();
 
    QRegExp rx(filter, Qt::CaseSensitive, QRegExp::Wildcard);
    QTextStream out(stdout);
    foreach (QString key, keys) {
        if (!rx.exactMatch(key))
            continue;
        if (provided && !backend->keyProvided(key))
            continue;
        if (longListing) {
            out << key << "\t" << backend->typeForKey(key) << "\t" << backend->pluginForKey(key) << "\t"
                << backend->constructionStringForKey(key) << "\t" << backend->docForKey(key) << "\n";
        } else
            out << key << "\n";
    }
    out.flush();

    exit(0);
    return app.exec();
}
