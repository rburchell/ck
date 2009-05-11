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

#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H

#include <QObject>
#include <QHash>
#include <QString>


// FIXME: these can be removed later
#include <QFileSystemWatcher>
#include <cdb.h>
#include <QVariant>

class PropertyProvider;
class PropertyHandle;


class PropertyManager : public QObject
{
    Q_OBJECT

private slots:
    void update_registry();
    void on_nameOwnerChanged(const QString &, const QString &, const QString &);

private:
    int registry_fd; ///< File descriptor of the opened CDB database
    QFileSystemWatcher *fsWatcher; ///< Watches the opened CDB database for changes
    struct cdb registry; ///< Opened CDB database

    bool lookupProvider(int provider_index, int &bus_kind, QString &bus_name);
    bool lookupProperty(const QString &key, int &provider_index, QVariant::Type &type,
            QString &typeName, QString &description);

    PropertyProvider* getProvider(int index);
    PropertyHandle* getHandle(const QString& path);

    static PropertyManager* instance();
    PropertyManager();
    ~PropertyManager();

    void commanderAppeared();
    void commanderDisappeared();

    void disableCommanding();

    /// Indicates whether the handles can be commanded by the Commander
    bool commandingEnabled;
    /// The provider representing the "overwriting provider", Context Commander
    PropertyProvider* commander;
    /// Indicates whether the handles are currently commanded by the Commander
    bool commanded;

    /// Hash of provider busnames to PropertyProvider objects.  The
    /// first hash stores the providers available on the session bus,
    /// while the second stores the ones available on the system bus.
    QHash<QString, PropertyProvider*> providers[2];
    /// Hash of property names to PropertyHandle objects.
    QHash<QString, PropertyHandle*> properties;

    QList<QString> listProperties ();

    // If you friend me, I friend you!

    friend class PropertyProvider;
    friend class PropertyHandle;

    friend class ContextPropertyPrivate;
    friend class ContextProperty;

};

#endif
