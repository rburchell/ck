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

#include <QCoreApplication>
#include <QDir>
#include <stdlib.h>
#include "contextregistryinfo.h"
#include "contextpropertyinfo.h"
#include "cdbwriter.h"
#include "fcntl.h"

/*!
   \page UpdatingContextProviders

   \brief The update tool (\c update-contextkit-providers) is used to regenerate the registry cache database.

   \section Overview

   Information about keys and providers is normally stored in a system directory in
   \b xml format. The xml (being xml) is slow to parse and not efficient as a storage format for
   data that is mostly static.

   It makes sense to store a cached version of the xml registry in a constant-database
   fast-access format and regenerate it when the xml data changes.

   Update tool does exactly that - it reads the xml registry and (re)generates a
   constant \b tiny-cdb database containing the cached version of the data in the registry.

   \section Usage

   The \c update-contextkit-providers binary, when lunched without parameters, will by default regenerate
   the database in the default installation prefix. Most likely: \c "/usr/share/contextkit/providers" .
   Obviously, for this to be successful, it needs to be launched with proper privileges.

   It's possible to override the registry directory with first parameter:

   \code
   $> update-contextkit-providers /some/path/to/registry
   \endcode

   In this case the xml will be read from \c "/some/path/to/registry" and the resulting
   database will be written to \c "/some/path/to/registry/cache.cdb" .

   Lastly, the \c "CONTEXT_PROVIDERS" environment variable can be used to specify
   a directory containing the registry.

   \section Implementation

   To ensure the registry consistency the regeneration is happening atomically - the
   new database is first written to a temp-named file and then moved-over the old one.
*/

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

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();

    // Check args etc
    QString path;

    // We first try to use first argument if present, then CONTEXT_PROVIDERS env,
    // lastly -- the compiled-in default path.
    if (args.size() > 1) {
        path = args.at(1);
        setenv("CONTEXT_PROVIDERS", path.toUtf8().constData(), 1);
    } else if (getenv("CONTEXT_PROVIDERS"))
        path = QString(getenv("CONTEXT_PROVIDERS"));
    else
        path = QString(DEFAULT_CONTEXT_PROVIDERS);

    printf("Updating from: '%s'\n", path.toUtf8().constData());

    // Force the xml backend
    ContextRegistryInfo *context = ContextRegistryInfo::instance("xml");

    QDir dir(path);
    checkDirectory(dir);
    QString tmpDbPath = dir.absoluteFilePath("cache-XXXXXX");
    QString finalDbPath = dir.absoluteFilePath("cache.cdb"); 
    QByteArray templ = tmpDbPath.toUtf8();

    char *tempPath = templ.data();
    CDBWriter writer(mkstemp(tempPath));
    chmod(tempPath, 0644);
    if (writer.isWritable() == false) {
        printf("ERROR: %s is not writable. No permissions?\n", templ.constData());
        exit(128);
    }

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

    if (fsync(writer.fileDescriptor()) != 0) {
        printf("ERROR: failed to fsync data on writer to %s.\n", templ.constData());
        exit(64);
    }

    writer.close();

    // Atomically rename
    if (rename(templ.constData(), finalDbPath.toUtf8().constData()) != 0) {
        printf("ERROR: failed to rename %s to %s.\n", templ.constData(), finalDbPath.toUtf8().constData());
        exit(64);
    }

    // All ok
    printf("Generated: '%s'\n", finalDbPath.toUtf8().constData());
    return 0;
}

