/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef PROPERTYHANDLE_H
#define PROPERTYHANDLE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QVariant>
#include <QHash>
#include <cdb.h>
#include <QFileSystemWatcher>

#include "keymanager.h"
#include "subscriberiface_p.h"

/* Handling context properties and their providers.

   A PropertyHandle represents a context property.  There is at most
   one PropertyHandle for each property, but more than one
   ContextProperty can point to a PropertyHandle.  A PropertyHandle
   points to a PropertyProvider object if there is a provider for it.

   A PropertProvider represents a context provider and manages the
   D-Bus connection to it.  A PropertyProvider can also represent the
   "no known provider" case.

   There is also a single PropertyManager object that ties everything
   together.  It reads the registry, for example.
   PropertyHandle and PropertyProvider objects as needed.

   PropertyHandle and PropertyProvider instances are never deleted;
   they stick around until the process is terminated.
 */

class PropertyHandle;
class PropertyProvider;
class PropertyManager;

class PropertyHandle : public KeyHandle
{
    Q_OBJECT

public:
    void subscribe();
    void unsubscribe();
    bool subscribed() const;

    QString providerName() const;

private:
    PropertyHandle(const QString& key);

    PropertyProvider *provider; ///< Provider of this property
    QVariant::Type type; ///< QVariant type of this property according to registry
    unsigned int subscribeCount; ///< Number of subscribed ContextProperty objects subscribed to this property

    void update_provider();

    friend class PropertyProvider;
    friend class PropertyManager;

};

class PropertyProvider : public QObject
{
    Q_OBJECT

public:
    void subscribe(PropertyHandle* handle);
    void unsubscribe(PropertyHandle* handle);

    QString getName() const;

private:
    PropertyProvider (int bus_kind, const QString& bus_name);
    ~PropertyProvider();

    SubscriberInterface *subscriber; ///< The DBUS interface for the provider
    QString dbusServiceString; ///< The DBUS service name of the provider (stored for debuging purposes)

private slots:
    void changeValues(const ValueSetMap& values, const QStringList& unknowns,
                      const bool processingSubscription = false);

    friend class PropertyManager;
};

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

    friend class KeyManager;

    friend class PropertyProvider;
    friend class PropertyHandle;

    friend class ContextPropertyPrivate;
    friend class ContextProperty;

};

#endif
