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

#ifndef INFOCDBBACKEND_H
#define INFOCDBBACKEND_H

#include <QStringList>
#include <QObject>
#include <QString>
#include <QFileSystemWatcher>
#include "cdbreader.h"
#include "infobackend.h"

/*!
    \class InfoCdbBackend

    \brief Implements the InfoBackend for reading data from a cdb database.

    This class is not exported in the public API. It does not cache any data
    to optimize the memory consumption. It's assumed that most data is anyways
    cached (as needed) in the ContextPropertyInfo and the cdb key-based access
    (no enumetation) is fast anyways. It observers the \c context-providers.cdb
    with a file system watcher.
*/

class InfoCdbBackend : public InfoBackend
{
    Q_OBJECT 

public:
    explicit InfoCdbBackend(QObject *parent = 0);

    virtual QString name() const;
    virtual QStringList listKeys() const;
    virtual QStringList listKeys(QString providername) const;
    virtual QStringList listProviders() const;
    virtual QString typeForKey(QString key) const;
    virtual QString docForKey(QString key) const;
    virtual QString providerForKey(QString key) const;
    virtual QString providerDBusTypeForKey(QString key) const;

    static QString databasePath();
    static QString databaseDirectory();
    static bool databaseExists();

private:
    /// A watched object obsering the database file. 
    /// Delivers syned notifications.
    QFileSystemWatcher watcher;

    /// The cdb reader object used to access the cdb database.
    CDBReader reader;

private slots:
    void onDatabaseFileChanged(const QString &path);
};

#endif // INFOCDBBACKEND_H
