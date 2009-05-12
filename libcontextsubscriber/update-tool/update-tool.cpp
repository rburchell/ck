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
    const char *path = dir.absolutePath().toUtf8().constData();

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

    /* Initialize some randomness */
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

    ContextRegistryInfo *context = ContextRegistryInfo::instance();
    foreach(QString key, context->listKeys()) {
        ContextPropertyInfo keyInfo(key);

        // Write value to list key
        writer.add("KEYLIST", key);

        // Write type
        writer.replace(key + ":KEYTYPE", keyInfo.type());

        // Write doc
        writer.replace(key + ":KEYDOC", keyInfo.doc());

        // Write provider
        writer.replace(key + ":PROVIDER", keyInfo.providerDBusName());
    }

    writer.close();
    rename(tmpDbPath.toUtf8().constData(), finalDbPath.toUtf8().constData()); 
}

