#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QTime>
#include "contextregistryinfo.h"
#include "contextpropertyinfo.h"
#include "cdbwriter.h"

/* Make sure the given directory exists, is readable etc. 
   If not, bail out with proper error. */
void checkDirectory(const QDir &dir)
{
    QByteArray utf8Data = dir.absolutePath().toUtf8();
    const char *path = utf8Data.constData();

    if (! dir.exists()) {
        printf ("ERROR: Directory '%s' does not exist!\n", path);
        exit (128);
    }

    if (! dir.isReadable()) {
        printf ("ERROR: Directory '%s' is not readable!\n", path);
        exit (128);
    }
}

/* Generates a temporary name for the database to be produced. */
QString genTempName(const QDir &dir)
{
    QString dbName = "context-providers-" + QString::number(qrand() % 10000) + ".cdb";
    return dir.absoluteFilePath(dbName);
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    // Force the xml backend
    ContextRegistryInfo *context = ContextRegistryInfo::instance("xml");

    // Initialize some randomness
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    const char *path = getenv("CONTEXT_PROVIDERS");
    if (! path) {
        printf ("ERROR: You need to specify CONTEXT_PROVIDERS env var!\n");
        exit (128);
    }

    QDir dir(path);
    checkDirectory(dir);
    QString tmpDbPath = genTempName(dir);
    QString finalDbPath = dir.absoluteFilePath("context-providers.cdb"); 
    qDebug() << tmpDbPath;

    CDBWriter writer(tmpDbPath);

    foreach(QString key, context->listKeys()) {
        ContextPropertyInfo keyInfo(key);

        // Write value to list key
        writer.add("KEYS", key);

        // Write type
        writer.replace(key + ":KEYTYPE", keyInfo.type());

        // Write doc
        writer.replace(key + ":KEYDOC", keyInfo.doc());

        // Write provider
        writer.replace(key + ":PROVIDER", keyInfo.providerDBusName());
    }

    foreach(QString provider, context->listProviders()) {
        // Write provider itself
        writer.add("PROVIDERS", provider);

        foreach(QString key, context->listKeys(provider)) {
            writer.add(provider + ":KEYS", key);
        }
    }

    writer.close();

    // Atomically rename
    QByteArray tmpDbUtf8Data = tmpDbPath.toUtf8();
    QByteArray finalDbUtf8Data = finalDbPath.toUtf8();

    rename(tmpDbUtf8Data.constData(), finalDbUtf8Data.constData()); 
}

