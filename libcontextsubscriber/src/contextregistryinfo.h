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

#ifndef CONTEXTREGISTRYINFO_H
#define CONTEXTREGISTRYINFO_H

#include <QVariant>
#include <QStringList>
#include <QObject>

class ContextRegistryInfo : public QObject
{
    Q_OBJECT

public:

    static ContextRegistryInfo* instance(const QString &backendName = "");

    QStringList listKeys() const;
    QStringList listKeys(QString providername) const;
    QStringList listKeysForPlugin(QString plugin) const;
    QStringList listProviders() const;
    QStringList listPlugins() const;
    QString backendName() const;

private:
    ContextRegistryInfo() {}; ///< Private constructor. Do not use.
    ContextRegistryInfo(const ContextRegistryInfo&); ///< Private constructor. Do not use.
    ContextRegistryInfo& operator=(const ContextRegistryInfo&); ///< Private operator. Do not use.

    /// Holds the actual pointer to the singelton instance.
    /// Mutex protected during creation.
    static ContextRegistryInfo* registryInstance;

protected:
    void connectNotify(const char *signal);


private slots:
    void onKeysChanged(const QStringList& currentKeys);
    void onKeysAdded(const QStringList& newKeys);
    void onKeysRemoved(const QStringList& removedKeys);
    void onListChanged();

signals:
    /// DEPRECATED use changed signal instead.
    /// Emitted when the registry changes. For performance reasons
    /// this is not a strict signal - it's emitted also even when no actual
    /// changes happened to the key content. For strict monitoring of a partular
    /// key use the \a ContextPropertyInfo.
    /// \param currentKeys List of all the keys that are now in the registry.
    void keysChanged(const QStringList& currentKeys);

    /// DEPRECATED use changed signal instead.
    /// Emitted when new keys become availible in the registry.
    /// The list contains only the new keys.
    /// \param newKeys New keys that appeared in the regisitry.
    void keysAdded(const QStringList& newKeys);

    /// DEPRECATED use changed signal instead.
    /// Emitted when keys disappear (are removed) from the registry.
    /// The list contains only the removed keys.
    /// \param removedKeys The list of removed keys.
    void keysRemoved(const QStringList& removedKeys);

    /// Emitted when the list of provided keys changes (keys were
    /// added or removed). This is not a strict signal - it's possible
    /// for the emission to happen even if no actual change
    /// happened.
    void changed();
};

#endif
