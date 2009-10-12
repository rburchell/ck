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
#include "contextproviderinfo.h"

class InfoCdbBackend : public InfoBackend
{
    Q_OBJECT

public:
    explicit InfoCdbBackend(QObject *parent = 0);

    virtual QString name() const;
    virtual QStringList listKeys() const;
    virtual QString typeForKey(QString key) const;
    virtual QString docForKey(QString key) const;
    virtual bool keyDeclared(QString key) const;
    virtual const QList<ContextProviderInfo> listProviders(QString key) const;

    static QString databaseDirectory();
    static QString databasePath();
    static bool databaseExists();

private:
    QFileSystemWatcher watcher; ///< A watched object obsering the database file. Delivers synced notifications.
    CDBReader reader; ///< The cdb reader object used to access the cdb database.
    void watchPathOrDirectory();
    void watchDirectory();
    void watchPath();
    static QStringList variantListToStringList(const QVariantList &l);

private slots:
    void onDatabaseFileChanged(const QString &path);
    void onDatabaseDirectoryChanged(const QString &path);
};

#endif // INFOCDBBACKEND_H
