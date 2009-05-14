/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "update-tool.h"

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
    QStringList args = app.arguments();

    // Check args etc
    QString path;

    // We first try to use first argument if present, then CONTEXT_PROVIDERS env, 
    // lastly -- the compiled-in default path.
    if (args.size() > 1)
        path = args.at(1);
    else if (getenv("CONTEXT_PROVIDERS"))
        path = QString(getenv("CONTEXT_PROVIDERS"));
    else
        path = QString(DEFAULT_REGISTRY_PATH);

    printf("Updating from: '%s'\n", path.toUtf8().constData());

    // Force the xml backend
    ContextRegistryInfo *context = ContextRegistryInfo::instance("xml");

    // Initialize some randomness
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));

    QDir dir(path);
    checkDirectory(dir);
    QString tmpDbPath = genTempName(dir);
    QString finalDbPath = dir.absoluteFilePath("context-providers.cdb"); 

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
        writer.replace(key + ":KEYPROVIDER", keyInfo.providerDBusName());

        // Write bus
        QDBusConnection::BusType t = keyInfo.providerDBusType(); 
        if (t == QDBusConnection::SessionBus)
            writer.replace(key + ":KEYBUS", "session");
        else if (t == QDBusConnection::SystemBus)
            writer.replace(key + ":KEYBUS", "system");
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
    printf("Generated: '%s'\n", finalDbUtf8Data.constData());
}

