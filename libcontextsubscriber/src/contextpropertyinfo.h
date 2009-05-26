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

class ContextPropertyInfo : public QObject
{
    Q_OBJECT

public:
    explicit ContextPropertyInfo(const QString &key, QObject *parent = 0);

    /// Destroys the object.
    virtual ~ContextPropertyInfo() {}

    QString key() const;
    QString doc() const;
    QString type() const;
    bool exists() const;
    
    QString providerDBusName() const;
    QDBusConnection::BusType providerDBusType() const;

private:
    QString keyName; //< The name of the key his ContextPropertyInfo represents.
    QString cachedType; //< Cached (stored) type of the key.
    QString cachedProvider; //< Cached (stored) provider of the key.
    QString cachedProviderDBusType; //< Cached (stored) provider dbus type of the key.

private slots:
    void onKeyDataChanged(const QString& key);

signals:
    /// Emitted when the provider of the key changes. The \a newProvider 
    /// contains the name of the new provider. This is a strict signal -
    /// it's emitted only when there was an actual change in the value.
    /// \param newProvider The DBus name of the new provider.
    void providerChanged(const QString& newProvider);

    /// Emitted when the bus type of provider of the key changes. The 
    /// \a newBusType is the type of new bus This is a strict signal -
    /// it's emitted only when there was an actual change in the value.
    /// \param newBusType The DBus bus type of the provider.
    void providerDBusTypeChanged(QDBusConnection::BusType newBusType);

    /// Emitted when the key type changes. The \a newType is the new type 
    /// of the introspected key. This is a strict signal - it's emitted only 
    /// when there was an actual change in the value.
    /// \param newType The new type of the key.
    void typeChanged(const QString& newType);

    /// Emitted when the key existance in the registry changes. The \a exists
    /// is the new state of the introspected key. This is a strict signal - it's 
    /// emitted only when there was an actual change in the state. Using this
    /// signal you can wait (watch) for various keys to become available.
    /// \param exists The new state of the key.
    void existsChanged(bool exists);
};

#endif // CONTEXTPROPERTYINFO_H
