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

#ifndef INFOBACKEND_H
#define INFOBACKEND_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include "contextproviderinfo.h"

class InfoBackend : public QObject
{
    Q_OBJECT

public:

    static InfoBackend* instance(const QString &backendName = "");

    /// Returns the name of the backend, ie: 'xml'.
    virtual QString name() const = 0;

    /// Returns the list of all the keys in the registry.
    virtual QStringList listKeys() const = 0;

    /// Returns the list of all the keys in the registry provided
    /// by the given \a plugin.
    virtual QStringList listKeysForPlugin(QString plugin) const = 0;

    /// Returns a list of all the unique plugins in the database.
    virtual QStringList listPlugins() const = 0;

    /// Returns a type for the given \a key.
    virtual QString typeForKey(QString key) const = 0;

    /// Returns the documentation for the given \a key name.
    virtual QString docForKey(QString key) const = 0;

    /// Returns the constructor plugin name for the given \a key name.
    virtual QString pluginForKey(QString key) const = 0;

    /// Returns the constructor plugin parameter for the given \a key name.
    virtual QString constructionStringForKey(QString key) const = 0;

    /// Returns true if the given key exists.
    virtual bool keyExists(QString key) const = 0;

    /// Returns true if the given key is provided (i.e., not a core
    /// property which nobody provides currently)
    virtual bool keyProvided(QString key) const = 0;

    virtual const QList<ContextProviderInfo> listProviders(QString key) const = 0;

signals:
    /// Emitted when key list changes. ContextRegistryInfo listens on that.
    void keysChanged(const QStringList& currentKeys);

    /// Emitted when new keys are added. ContextRegistryInfo listens on that.
    void keysAdded(const QStringList& newKeys);

    /// Emitted when keys are removed. ContextRegistryInfo listens on that.
    void keysRemoved(const QStringList& removedKeys);

    /// Emitted when key data changes. ContextPropertyInfo instances listen on that.
    void keyDataChanged(const QString& key);

    void changed(const QString& key);

protected:
    virtual void connectNotify(const char *signal);
    virtual void disconnectNotify(const char *signal);

private:
    int connectCount; ///< Number of connections to signals. Used to optimized signal emission when 0.

    InfoBackend(QObject *parent = 0);

    /// Private constructor. Do not use.
    InfoBackend(const InfoBackend&);

    void checkAndEmitKeysAdded(const QStringList &currentKeys, const QStringList &oldKeys);
    void checkAndEmitKeysRemoved(const QStringList &currentKeys, const QStringList &oldKeys);
    void checkAndEmitKeysChanged(const QStringList &currentKeys, const QStringList &oldKeys);

    /// Private operator. Do not use.
    InfoBackend& operator=(const InfoBackend&);

    static InfoBackend* backendInstance; ///< Holds a pointer to the instance of the singelton.
    static void destroyInstance();

    friend class InfoXmlBackend;
    friend class InfoCdbBackend;
    friend class InfoTestBackend;
    friend class InfoXmlBackendUnitTest;
    friend class InfoCdbBackendUnitTest;
    friend class InfoBackendUnitTest;

};

#endif // INFOBACKEND_H
