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

#ifndef INFOXMLBACKEND_H
#define INFOXMLBACKEND_H

#include <QStringList>
#include <QObject>
#include <QFileInfo>
#include <QString>
#include <QHash>
#include <QFileSystemWatcher>
#include "infobackend.h"
#include "infokeydata.h"

/*!
    \class InfoXmlBackend

    \brief Implements the InfoBackend for reading data from a directory
    with xml files.

    This class is not exported in the public API. It keeps all the data cached 
    in the memory. It's assumed that this backend is not going to be used live in
    production systems and does not need to be ultra-fast (instead, implementation
    simplicity and corectness are preffered). For fast backend see the InfoCdbBackend.
*/

class InfoXmlBackend : public InfoBackend
{
    Q_OBJECT 

public:
    explicit InfoXmlBackend(QObject *parent = 0);

    virtual QString name() const;
    virtual QStringList listKeys() const;
    virtual QStringList listKeys(QString providername) const;
    virtual QStringList listProviders() const;
    virtual QString typeForKey(QString key) const;
    virtual QString docForKey(QString key) const;
    virtual QString providerForKey(QString key) const;
    virtual QString providerDBusTypeForKey(QString key) const;
    
    static QString registryPath();

private slots:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &path);

private:
    /// A watched object obsering the database file. 
    /// Delivers syned notifications.
    QFileSystemWatcher watcher;

    /// This hash contains the full state of registry in memory.
    QHash <QString, InfoKeyData> keyDataHash;

    /// The number of xml files we parsed in last registry update.
    int countOfFilesInLastParse;

    void regenerateKeyDataList();
    void readKeyDataFromXml(const QFileInfo &f);
};

#endif // INFOXMLBACKEND_H
