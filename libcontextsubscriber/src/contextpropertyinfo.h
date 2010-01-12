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
#include <QMutex>
#include "contextproviderinfo.h"
#include "contexttypeinfo.h"

class ContextPropertyInfo : public QObject
{
    Q_OBJECT

public:
    enum ResolutionStrategy { LastValue };

    explicit ContextPropertyInfo(const QString &key, QObject *parent = 0);

    /// Destroys the object.
    virtual ~ContextPropertyInfo() {}

    QString key() const;
    QString doc() const;
    QString type() const;
    ContextTypeInfo typeInfo() const;
    bool exists() const;
    bool declared() const;
    bool provided() const;

    QString providerDBusName() const;
    QDBusConnection::BusType providerDBusType() const;

    QString plugin() const;
    QString constructionString() const;
    virtual const QList<ContextProviderInfo> providers() const;
    ResolutionStrategy resolutionStrategy() const;

protected:
    virtual void connectNotify(const char *signal);

private:
    QString keyName; ///< The name of the key his ContextPropertyInfo represents.
    QString cachedDoc; ///< Cached documentation of the key.
    ContextTypeInfo cachedTypeInfo; ///< Cached (stored) type info of the key.
    bool cachedDeclared; ///< Cached state of the key (existance).
    QList<ContextProviderInfo> cachedProviders; ///< Cached list of providers for this key.
    mutable QMutex cacheLock; ///< Lock for the cache.

    QString providerDBusName_i() const;
    QDBusConnection::BusType providerDBusType_i() const;
    QString plugin_i() const;
    QString constructionString_i() const;

private Q_SLOTS:
    void onKeyChanged(const QString& key);

Q_SIGNALS:
    /// DEPRECATED, use changed() signal.
    /// Emitted when the provider of the key changes. The \a newProvider
    /// contains the name of the new provider. This is a strict signal -
    /// it's emitted only when there was an actual change in the value.
    /// \param newProvider The DBus name of the new provider.
    void providerChanged(const QString& newProvider);

    /// DEPRECATED, use changed() signal.
    /// Emitted when the bus type of provider of the key changes. The
    /// \a newBusType is the type of new bus This is a strict signal -
    /// it's emitted only when there was an actual change in the value.
    /// \param newBusType The DBus bus type of the provider.
    void providerDBusTypeChanged(QDBusConnection::BusType newBusType);

    /// DEPRECATED, use changed() signal.
    /// Emitted when the key type changes. The \a newType is the new type
    /// of the introspected key. This is a strict signal - it's emitted only
    /// when there was an actual change in the value.
    /// \param newType The new type of the key.
    void typeChanged(const QString& newType);

    /// DEPRECATED, use changed() signal.
    /// Emitted when the key existance in the registry changes. The \a exists
    /// is the new state of the introspected key. This is a strict signal - it's
    /// emitted only when there was an actual change in the state. Using this
    /// signal you can wait (watch) for various keys to become available.
    /// \param exists The new state of the key.
    void existsChanged(bool exists);

    /// DEPRECATED, use changed() signal.
    /// Emitted when the key gets a provider or loses a provider. The \a provided
    /// is the new state of the introspected key. This is a strict signal - it's
    /// emitted only when there was an actual change in the state. Using this
    /// signal you can wait (watch) for various keys to become available.
    /// \param provided The new state of the key.
    void providedChanged(bool provided);

    /// DEPRECATED, use changed() signal.
    /// Emitted when the libcontextsubscriber plugin providing the key
    /// changes, or the construction parameter to give to the plugin
    /// changes.. The \a plugin is the name of the new plugin
    /// providing the key and the \a constructionString is the new
    /// construction parameter to give to the plugin.
    void pluginChanged(QString plugin, QString constructionString);

    /// Emitted when any of the key parameters/data changes.
    /// This is not a strict signal - it might be emitted even when
    /// no actual change happened.
    void changed(QString key);

    friend class ContextPropertyInfoUnitTest;
};

#endif // CONTEXTPROPERTYINFO_H
