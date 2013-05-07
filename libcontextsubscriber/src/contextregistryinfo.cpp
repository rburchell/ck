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

#include "contextregistryinfo.h"
#include "infobackend.h"
#include "sconnect.h"
#include "logging.h"
#include "loggingfeatures.h"
#include "contextproviderinfo.h"
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>

/*!
    \class ContextRegistryInfo

    \brief A class to introspect the registry contents.

    This is a singelton class used to obtain information about the keys (properties)
    in the registry database. The information can be provided either from xml files
    or from a cdb database. It's possible to list all the keys in the registry and
    also list all keys belonging to a one particular provider.
*/

ContextRegistryInfo* ContextRegistryInfo::registryInstance = NULL;

/* Public */

/// Returns the singleton instance of the ContextRegistryInfo. The object
/// is constructed automaticall on first access.
/// \param backendName the optional name of the backend to use (force).

ContextRegistryInfo* ContextRegistryInfo::instance(const QString &backendName)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (! registryInstance) {
        InfoBackend::instance(backendName);
        registryInstance = new ContextRegistryInfo;

        InfoBackend* infoBackend = InfoBackend::instance();

        sconnect(infoBackend, SIGNAL(keysChanged(QStringList)),
                 registryInstance, SLOT(onKeysChanged(QStringList)));

        sconnect(infoBackend, SIGNAL(keysAdded(QStringList)),
                 registryInstance, SLOT(onKeysAdded(QStringList)));

        sconnect(infoBackend, SIGNAL(keysRemoved(QStringList)),
                 registryInstance, SLOT(onKeysRemoved(QStringList)));

        sconnect(infoBackend, SIGNAL(listChanged()),
                 registryInstance, SLOT(onListChanged()));

        // Move the backend to the main thread
        registryInstance->moveToThread(QCoreApplication::instance()->thread());
    }

    return registryInstance;
}

/// Returns the list of all the keys currently availible in the registry.
QStringList ContextRegistryInfo::listKeys() const
{
    return InfoBackend::instance()->listKeys();
}

/// Returns the list of all the keys associated with the given provider.
QStringList ContextRegistryInfo::listKeys(QString providerName) const
{
    contextWarning() << F_DEPRECATION << "ContextRegistryInfo::listKeys(QString provider) is deprecated.";

    QSet<QString> keys;

    Q_FOREACH (QString key, listKeys()) {
        Q_FOREACH (ContextProviderInfo info, InfoBackend::instance()->providersForKey(key)) {
            if (info.plugin == "contextkit-dbus" &&
                info.constructionString.split(":").last() == providerName) {
                keys.insert(key);
            }
        }
    }

    return keys.toList();
}

/// DEPRECATED Returns the list of all the keys associated with the given plugin.
QStringList ContextRegistryInfo::listKeysForPlugin(QString plugin) const
{
    contextWarning() << F_DEPRECATION << "ContextRegistryInfo::listKeysForPlugin() is deprecated.";

    QSet<QString> keys;

    Q_FOREACH (QString key, listKeys()) {
        Q_FOREACH (ContextProviderInfo info, InfoBackend::instance()->providersForKey(key)) {
            if (info.plugin == plugin) {
                keys.insert(key);
            }
        }

    }

    return keys.toList();
}

/// DEPRECATED Returns the list of all unique providers in the registry.
/// The lists consist of strings with dbus names of the providers.
QStringList ContextRegistryInfo::listProviders() const
{
    contextWarning() << F_DEPRECATION << "ContextRegistryInfo::listProviders() is deprecated.";

    QSet<QString> providers;

    Q_FOREACH (QString key, listKeys()) {
        Q_FOREACH (ContextProviderInfo info, InfoBackend::instance()->providersForKey(key)) {
            if (info.plugin == "contextkit-dbus") {
                providers.insert(info.constructionString.split(":").last());
            }
        }
    }

    return providers.toList();
}

/// DEPRECATED Returns the list of all unique plugins in the registry.
QStringList ContextRegistryInfo::listPlugins() const
{
    contextWarning() << F_DEPRECATION << "ContextRegistryInfo::listPlugins() is deprecated.";

    QSet<QString> plugins;

    Q_FOREACH (QString key, listKeys()) {
        Q_FOREACH (ContextProviderInfo info, InfoBackend::instance()->providersForKey(key)) {
            plugins.insert(info.plugin);
        }
    }

    return plugins.toList();
}

/// Returns the name of the currently used registry backend. Ie. "cdb" or "xml".
QString ContextRegistryInfo::backendName() const
{
    return InfoBackend::instance()->name();
}

/* Slots */

/// This is connected to the \a keysChanged of the actual info backend instance.
/// Will be removed when deprecated keysChanged() signal is removed.
void ContextRegistryInfo::onKeysChanged(const QStringList& currentKeys)
{
    Q_EMIT keysChanged(currentKeys);
}

/// This is connected to the \a keysAdded of the actual info backend instance.
/// Will be removed when deprecated keysAdded() signal is removed.
void ContextRegistryInfo::onKeysAdded(const QStringList& newKeys)
{
    Q_EMIT keysAdded(newKeys);
}

/// This is connected to the \a keysRemoved of the actual info backend instance.
/// Will be removed when deprecated keysRemoved() signal is removed.
void ContextRegistryInfo::onKeysRemoved(const QStringList& removedKeys)
{
    Q_EMIT keysRemoved(removedKeys);
}

/// Called when people connect to signals. Used to emit deprecation warnings
/// when people connect to deprecated signals.
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
void ContextRegistryInfo::connectNotify(const char *_signal)
#else
void ContextRegistryInfo::connectNotify(const QMetaMethod &_signal)
#endif
{
    QObject::connectNotify(_signal);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QByteArray signal = _signal;
#else
    QByteArray signal = _signal.methodSignature();
#endif

    if (signal == SIGNAL(keysChanged(QStringList)))
        contextWarning() << F_DEPRECATION << "ContextRegistryInfo::keysChanged signal is deprecated.";
    else if (signal == SIGNAL(keysAdded(QStringList)))
        contextWarning() << F_DEPRECATION << "ContextRegistryInfo::keysAdded signal is deprecated.";
    else if (signal == SIGNAL(keysRemoved(QStringList)))
        contextWarning() << F_DEPRECATION << "ContextRegistryInfo::keysRemoved signal is deprecated.";
}

/// This is connected to the \a listChanged of the actual info backend instance.
/// Gets called when the list of keys changes.
void ContextRegistryInfo::onListChanged()
{
    Q_EMIT changed();
}
