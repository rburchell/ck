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

#include "subscriberinterface.h"

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

class PropertyHandle : public QObject
{
    Q_OBJECT

public:
    void subscribe();
    void unsubscribe();
    bool subscribed() const;

    QString providerName() const;

    QString key;
    QVariant value;

    // TODO: Remove these when introspection is ready
    QString typeName; ///< Type name of this property.
    QString description; ///< Documentation describing this property.

signals:
    void valueChanged();

private:
    PropertyHandle(const QString& key);

    PropertyProvider *provider; ///< Provider of this property
    QVariant::Type type; ///< QVariant type of this property according to registry
    unsigned int subscribeCount; ///< Number of subscribed ContextProperty objects subscribed to this property

    void update_provider();

    friend class PropertyProvider;
    friend class PropertyManager;
};


#endif
