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

#ifndef CONTEXTPROPERTYINFO_H
#define CONTEXTPROPERTYINFO_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <QDBusConnection>

/*!
  \class ContextPropertyInfo

  \brief A class to introspect a context property details.

  This class is used to obtain information about a given key in the context registry. 
  The information can be provided either from xml files or from a cdb database. 
  It's possible to query the type, the provider and the documentation of the given 
  key/property.

*/

class ContextPropertyInfo : public QObject
{
    Q_OBJECT

public:
    explicit ContextPropertyInfo(const QString &key, QObject *parent = 0);

    virtual ~ContextPropertyInfo() {}

    QString key() const;
    QString doc() const;
    QString type() const;
    
    QString providerDBusName() const;
    QDBusConnection::BusType providerDBusType() const;

private:
    QString keyName;
    QString cachedType;
    QString cachedProvider; 
    QString cachedProviderDBusType;

private slots:
    void onKeyDataChanged(QString key);

signals:
    /// Emitted when the provider of the key changes. The \a newProvider 
    /// contains the name of the new provider. This is a strict signal -
    /// it's emitted only when there was an actual change in the value.
    /// \param newProvider The DBus name of the new provider.
    void providerChanged(QString newProvider);

    /// Emitted when the bus type of provider of the key changes. The 
    /// \a newBusType is the type of new bus This is a strict signal -
    /// it's emitted only when there was an actual change in the value.
    /// \param newBusType The DBus bus type of the provider.
    void providerDBusTypeChanged(QDBusConnection::BusType newBusType);

    /// Emitted when the key type changes. The \a newType is the new type 
    /// of the introspected key. This is a strict signal - it's emitted only 
    /// when there was an actual change in the value.
    /// \param newType The new type of the key.
    void typeChanged(QString newType);
};

#endif // CONTEXTPROPERTYINFO_H
