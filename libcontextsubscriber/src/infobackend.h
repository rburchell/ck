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
#include "contexttypeinfo.h"

class InfoBackend : public QObject
{
    Q_OBJECT

public:

    static InfoBackend* instance(const QString &backendName = "");

    /// Returns the name of the backend, ie: 'xml'.
    virtual QString name() const = 0;

    /// Returns the list of all the keys in the registry.
    virtual QStringList listKeys() const = 0;

    /// Returns the type information for the given \a key.
    virtual ContextTypeInfo typeInfoForKey(QString key) const = 0;

    /// Returns the documentation for the given \a key name.
    virtual QString docForKey(QString key) const = 0;

    /// Returns true if the given key exists.
    virtual bool keyDeclared(QString key) const = 0;

    /// Returns a list of providers for the given key.
    virtual const QList<ContextProviderInfo> providersForKey(QString key) const = 0;

signals:
    /// Emitted when key list changes. ContextRegistryInfo listens on that.
    void keysChanged(const QStringList& currentKeys);

    /// Emitted when new keys are added. ContextRegistryInfo listens on that.
    void keysAdded(const QStringList& newKeys);

    /// Emitted when keys are removed. ContextRegistryInfo listens on that.
    void keysRemoved(const QStringList& removedKeys);

    /// Emitted when key data/info changes. ContextPropertyInfo instances listen on that.
    void keyChanged(const QString& key);

    /// Emitted when the key list changes. ContextRegistryInfo listens on that.
    void listChanged();

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
    void checkAndEmitKeyChanged(const QStringList &currentKeys, const QStringList &oldKeys);

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
