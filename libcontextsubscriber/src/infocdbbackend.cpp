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
#include <QMutex>
#include <QHash>
#include "sconnect.h"
#include "infocdbbackend.h"

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
    qDebug() << "CDB backend with database" << InfoCdbBackend::databasePath();

    if (reader.isReadable() == false) {
        qDebug() << InfoCdbBackend::databasePath() << "is not readable! Bailing out.";
        return;
    }

    watcher.addPath(InfoCdbBackend::databasePath());

    sconnect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(onDatabaseFileChanged(QString)));
}

/// Returns 'cdb'.
QString InfoCdbBackend::name() const
{
    return QString("cdb");
}

QStringList InfoCdbBackend::listKeys() const
{
    return reader.valuesForKey("KEYS");
}

QStringList InfoCdbBackend::listKeys(QString providername) const
{
    return reader.valuesForKey(providername + ":KEYS");
}

QStringList InfoCdbBackend::listProviders() const
{
    return reader.valuesForKey("PROVIDERS");
}

QString InfoCdbBackend::typeForKey(QString key) const
{
    return reader.valueForKey(key + ":KEYTYPE");
}

QString InfoCdbBackend::docForKey(QString key) const
{
    return reader.valueForKey(key + ":KEYDOC");
}

QString InfoCdbBackend::providerForKey(QString key) const
{
    return reader.valueForKey(key + ":KEYPROVIDER");
}

QString InfoCdbBackend::providerDBusTypeForKey(QString key) const
{
    return reader.valueForKey(key + ":KEYBUS");
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

    return QString(regpath) + "cache.cdb";
}

/// Returns full path to the directory containing the database.
/// Takes the \c CONTEXT_PROVIDERS env varialbe into account.
QString InfoCdbBackend::databaseDirectory()
{
    const char *regpath = getenv("CONTEXT_PROVIDERS");
    if (! regpath)
        regpath = DEFAULT_CONTEXT_PROVIDERS;

    return QString(regpath);
}

/* Slots */

/// Called when the database changes. Reopens the database and emits 
/// the change signals. If database does not exist it bails out but keeps observing.
void InfoCdbBackend::onDatabaseFileChanged(const QString &path)
{
    QStringList oldKeys = listKeys();

    qDebug() << InfoCdbBackend::databasePath() << "changed, re-opening database.";

    reader.reopen();
    if (reader.isReadable() == false) {
        qDebug() << InfoCdbBackend::databasePath() << "is not readable! Bailing out.";
        emit keysChanged(QStringList());
        return;
    }
  
    // QFileSystemWatcher stops monitoring files and directories once 
    // they have been removed from disk. Need to add again if we had a move ops.
    if (! watcher.files().contains(InfoCdbBackend::databasePath()))
        watcher.addPath(InfoCdbBackend::databasePath());
        
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


