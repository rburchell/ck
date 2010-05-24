/*
 * Copyright (C) 2008 Nokia Corporation.
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

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <sys/stat.h>
#include <stdlib.h>
#include "sconnect.h"
#include "infocdbbackend.h"
#include "logging.h"
#include "loggingfeatures.h"
#include "contextproviderinfo.h"

/*!
    \class InfoCdbBackend

    \brief Implements the InfoBackend for reading data from a cdb database.

    This class is not exported in the public API. It does not cache any data
    to optimize the memory consumption. It's assumed that most data is anyways
    cached (as needed) in the ContextPropertyInfo and the cdb key-based access
    (no enumetation) is fast anyways. It observers the \c cache.cdb
    with a file system watcher.
*/

InfoCdbBackend::InfoCdbBackend(QObject *parent)
    : InfoBackend(parent), reader(InfoCdbBackend::databasePath()), lastInode(0)
{
    contextDebug() << F_CDB << "Initializing cdb backend with database:" << InfoCdbBackend::databasePath();

    sconnect(&watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDatabaseDirectoryChanged(QString)));

    watcher.addPath(InfoCdbBackend::databaseDirectory());

    checkCompatibility();

    struct stat buffer;
    if (!stat(InfoCdbBackend::databasePath().toAscii(), &buffer))
        lastInode = buffer.st_ino;
    else
        lastInode = 0;
}

/// Returns 'cdb'.
QString InfoCdbBackend::name() const
{
    return QString("cdb");
}

QStringList InfoCdbBackend::variantListToStringList(const QVariantList &l)
{
    QStringList ret;
    Q_FOREACH (QVariant v, l)
        ret << v.toString();

    return ret;
}

QStringList InfoCdbBackend::listKeys() const
{
    if (databaseCompatible)
        return variantListToStringList(reader.valuesForKey("KEYS"));
    else
        return QStringList();
}

QString InfoCdbBackend::docForKey(QString key) const
{
    if (databaseCompatible)
        return reader.valueForKey(key + ":KEYDOC").toString();
    else
        return QString();
}

bool InfoCdbBackend::keyDeclared(QString key) const
{
    if (!databaseCompatible)
        return false;
    else if (reader.valuesForKey("KEYS").contains(key))
        return true;
    else
        return false;
}

bool InfoCdbBackend::keyDeprecated(QString key) const
{
    if (databaseCompatible)
        return reader.valueForKey(key + ":KEYDEPRECATED").toBool();
    else
        return false;
}

/// Returns true if the database file is present.
bool InfoCdbBackend::databaseExists()
{
    QFile file(databasePath());
    return file.exists();
}

/// Returns the full path to the database.
/// Takes the \c CONTEXT_PROVIDERS env variable into account.
QString InfoCdbBackend::databasePath()
{
    const char *regpath = getenv("CONTEXT_PROVIDERS");
    if (! regpath)
        regpath = DEFAULT_CONTEXT_PROVIDERS;

    return QDir(regpath).filePath("cache.cdb");
}

/// Returns the full path to the database directory.
/// Takes the \c CONTEXT_PROVIDERS env variable into account.
QString InfoCdbBackend::databaseDirectory()
{
    const char *regpath = getenv("CONTEXT_PROVIDERS");
    if (! regpath)
        regpath = DEFAULT_CONTEXT_PROVIDERS;

    return QString(regpath);
}

/* Private */

/// Update the database compatibility field.
void InfoCdbBackend::checkCompatibility()
{
    if (!reader.isReadable())
        databaseCompatible = false;
    else {
        QString version = reader.valueForKey("VERSION").toString();
        if (version != "" && version != BACKEND_COMPATIBILITY_NAMESPACE) {
            databaseCompatible = false;
            contextWarning() << F_CDB << "Incompatible database version:" << version;
        } else
            databaseCompatible = true;
    }
}

/* Slots */

/// Called when the database directory changes. Reopens the database and emits
/// the change signal if the inode of the database has been modified.
void InfoCdbBackend::onDatabaseDirectoryChanged(const QString &path)
{
    contextDebug() << F_CDB << InfoCdbBackend::databaseDirectory() << "Directory changed.";

    struct stat buffer;
    quint64 inode;

    if (!stat(InfoCdbBackend::databasePath().toAscii(), &buffer))
        inode = buffer.st_ino;
    else
        inode = 0;

    if (lastInode != inode) {
        lastInode = inode;

        QStringList oldKeys = listKeys();

        contextDebug() << F_CDB << InfoCdbBackend::databasePath() << "File changed, re-opening database.";

        reader.reopen();

        // Check the version
        if (reader.isReadable())
            checkCompatibility();

        // If nobody is watching us anyways, drop out now and skip
        // the further processing. This could be made more granular
        // (ie. in many cases nobody will be watching on added/removed)
        // but will be watching on changed.
        if (connectCount == 0)
            return;

        QStringList currentKeys = listKeys();
        // Emissions
        checkAndEmitKeysAdded(currentKeys, oldKeys); // DEPRECATED emission
        checkAndEmitKeysRemoved(currentKeys, oldKeys); // DEPRECATED emission
        Q_EMIT keysChanged(listKeys()); // DEPRECATED emission

        Q_EMIT listChanged();
        checkAndEmitKeyChanged(currentKeys, oldKeys);
    }

}

const QList<ContextProviderInfo> InfoCdbBackend::providersForKey(QString key) const
{
    QVariant providers = reader.valueForKey(key + ":PROVIDERS");
    QList<ContextProviderInfo> lst;

    if (!databaseCompatible)
        return lst;

    Q_FOREACH (QVariant variant, providers.toList())
        lst << ContextProviderInfo(variant.toHash().value("plugin").toString(),
                                   variant.toHash().value("constructionString").toString());

    return lst;
}

ContextTypeInfo InfoCdbBackend::typeInfoForKey(QString key) const
{
    return ContextTypeInfo(reader.valueForKey(key + ":KEYTYPEINFO"));
}
