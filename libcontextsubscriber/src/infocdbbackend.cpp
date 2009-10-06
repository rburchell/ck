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

#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <stdlib.h>
#include <QHash>
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
    : InfoBackend(parent), reader(InfoCdbBackend::databasePath())
{
    contextDebug() << F_CDB << "Initializing cdb backend with database:" << InfoCdbBackend::databasePath();

    sconnect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(onDatabaseFileChanged(QString)));
    sconnect(&watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDatabaseDirectoryChanged(QString)));

    watchPathOrDirectory();
}

/// Returns 'cdb'.
QString InfoCdbBackend::name() const
{
    return QString("cdb");
}

QStringList InfoCdbBackend::variantListToStringList(const QVariantList &l)
{
    QStringList ret;
    foreach (QVariant v, l)
        ret << v.toString();

    return ret;
}


QStringList InfoCdbBackend::listKeys() const
{
    return variantListToStringList(reader.valuesForKey("KEYS"));
}

QStringList InfoCdbBackend::listKeysForPlugin(QString plugin) const
{
    return variantListToStringList(reader.valuesForKey(plugin + ":KEYS"));
}

QStringList InfoCdbBackend::listPlugins() const
{
    return variantListToStringList(reader.valuesForKey("PLUGINS"));
}

QString InfoCdbBackend::typeForKey(QString key) const
{
    return reader.valueForKey(key + ":KEYTYPE").toString();
}

QString InfoCdbBackend::docForKey(QString key) const
{
    return reader.valueForKey(key + ":KEYDOC").toString();
}

QString InfoCdbBackend::pluginForKey(QString key) const
{
    return reader.valueForKey(key + ":KEYPLUGIN").toString();
}

QString InfoCdbBackend::constructionStringForKey(QString key) const
{
    return reader.valueForKey(key + ":KEYCONSTRUCTIONSTRING").toString();
}

bool InfoCdbBackend::keyExists(QString key) const
{
    if (reader.valuesForKey("KEYS").contains(key))
        return true;
    else
        return false;
}

bool InfoCdbBackend::keyProvided(QString key) const
{
    QString plugin = reader.valueForKey(key + ":KEYPLUGIN").toString();
    if (plugin == "")
        return false;

    return true;
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

/// Start watching the database direcory for changes.
void InfoCdbBackend::watchDirectory()
{
    if (! watcher.directories().contains(InfoCdbBackend::databaseDirectory()))
        watcher.addPath(InfoCdbBackend::databaseDirectory());
}

/// Start watching the database file for changes.
void InfoCdbBackend::watchPath()
{
    if (! watcher.files().contains(InfoCdbBackend::databasePath()))
        watcher.addPath(InfoCdbBackend::databasePath());
}

/// Depending on our readability status, watch either path or the
/// directory.
void InfoCdbBackend::watchPathOrDirectory()
{
    if (reader.isReadable() == false) {
        contextDebug() << F_CDB << InfoCdbBackend::databasePath() << "is not readable. Watching dir instead.";
        watchDirectory();
    } else {
        watchPath();
    }
}

/* Slots */

/// Called when the database changes. Reopens the database and emits
/// the change signals. If database does not exist it bails out but keeps observing.
void InfoCdbBackend::onDatabaseFileChanged(const QString &path)
{
    QStringList oldKeys = listKeys();

    contextDebug() << F_CDB << InfoCdbBackend::databasePath() << "changed, re-opening database.";

    reader.reopen();
    watchPathOrDirectory();

    // If nobody is watching us anyways, drop out now and skip
    // the further processing. This could be made more granular
    // (ie. in many cases nobody will be watching on added/removed)
    // but will be watching on changed.
    if (connectCount == 0)
        return;

    QStringList currentKeys = listKeys();

    // Emissions
    checkAndEmitKeysAdded(currentKeys, oldKeys);
    checkAndEmitKeysRemoved(currentKeys, oldKeys);
    emit keysChanged(listKeys());
    checkAndEmitKeysChanged(currentKeys, oldKeys);
}

/// Called when the directory with cache.db chanes. We start to observe this
/// directory only when we don't have the cache.db in the first place.
void InfoCdbBackend::onDatabaseDirectoryChanged(const QString &path)
{
    if (reader.isReadable())
        return;

    onDatabaseFileChanged(path);
}

const QList<ContextProviderInfo> InfoCdbBackend::listProviders(QString key) const
{
     return QList<ContextProviderInfo> ();
}

