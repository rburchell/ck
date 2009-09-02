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

#include "contextpropertyinfo.h"
#include "infobackend.h"
#include "sconnect.h"

#include <QReadLocker>
#include <QWriteLocker>

/*!
    \page Introspection

    \brief The Context Framework maintains a registry defining which
    context properties are currently provided and by whom. The
    introspection API of libcontextsubscriber allows you to inspect
    the current state of the registry and observe its changes.

    \section Overview

    The introspection is provided via two classes: ContextRegistryInfo and
    ContextPropertyInfo.

    ContextRegistryInfo provides a high-level view to the registry
    contents.  You can use it to obtain info about the list of
    currently available keys or e.g. get a list of keys for one
    particular provider.  ContextRegistryInfo is a singleton instance
    which is created on the first access.

    ContextPropertyInfo is used to obtain metadata about one
    particular key. Once created, it can be used to retrieve the type
    and provider information (DBus bus type and name) of the
    introspected key. It also provides a couple of useful signals for
    watching changes happening to a key.

    \section Usage

    \code
    // To get a list of all keys in the registry
    ContextRegistryInfo *context = ContextRegistryInfo::instance();
    QStringList currentKeys = context->listKeys();
    \endcode

    Using the ContextPropertyInfo is even more straight-forward.

    \code
    // To check the type of a particular key
    ContextPropertyInfo propInfo("Battery.ChargeLevel");
    QString propType = propInfo.type();
    \endcode

    The introspection API in general never asserts (never fails). It'll return empty strings
    on errors or if data is missing. For example:

    \code
    ContextPropertyInfo propInfo("Something.That.Doesnt.Exist");
    propInfo.type();     //  ...returns empty string
    propInfo.doc();      //  ...returns empty string
    propInfo.provider();  //  ...returns empty string
    \endcode

    You can use this functionality to wait for keys to become available in the registry.
    Just create a ContextPropertyInfo for a key that you're expecting to become present
    and connect to the /c existsChanged signal.

    \code
    ContextPropertyInfo propInfo("Something.That.Doesnt.Exist");
    propInfo.exists(); // false
    // Connect something to the existsChanged signal.
    \endcode

    \section xmlvscdb XML vs.CDB

    When the introspection API is first used, a backend choice is being made. \b CDB backend
    (reading data from \c 'cache.cdb' ) is used if the tiny database cache file exists
    in the registry. The standard (slower) \b XML backend is used in other cases.

    It's possible to force a usage of a particular backend.
    This can be done by calling the \c instance method with a string name of the backend:

    \code
    ContextRegistryInfo::instance("cdb"); // or "xml"
    \endcode

    This needs to be done early enough before the introspection API is first used.
    For more information about the \b xml and \cdb backends read the \ref UpdatingContextProviders page.

*/

/*!
    \page MigratingFromDuiValueSpace

    \brief libcontextsubscriber is a replacement library for DuiValueSpace which is deprecated.

    DuiValueSpace, the old subscription library providing the keys, is deprecated. This library
    is a replacement for it, providing better API and better implementation while maintaining the
    same core ideas and structure.

    \section quicklook A quick look

    The following code for creating a handle for a context property:

    \code
    DuiValueSpaceItem topEdge("Context.Screen.TopEdge");
    QObject::connect(&topEdge, SIGNAL(valueChanged()),
                     this, SLOT(topEdgeChanged()));
    \endcode

    becomes:

    \code
    ContextProperty topEdge("Screen.TopEdge");
    QObject::connect(&topEdge, SIGNAL(valueChanged()),
                     this, SLOT(topEdgeChanged()));
    \endcode

    The following code for listing the available context keys:

    \code
    DuiValueSpaceItem::listKeys();
    \endcode

    becomes:

    \code
    ContextRegistryInfo::instance()->listKeys();
    \endcode

    \section prefix The Context. prefix

    In \b DuiValueSpace and accompanying packages, the properties used to
    have a "Context." prefix. For example:

    \code
    Context.Screen.TopEdge
    Context.Screen.IsCovered
    \endcode

    This 'Context.' has been dropped now from \b libcontextsubscriber and
    all the provider packages. Providers now explicitly provide properties
    with keys like:

    \code
    Screen.TopEdge
    Screen.IsCovered
    \endcode

    For compatibility reasons the 'Context.' prefix is still supported in
    newer releases of \b DuiValueSpace. The \b DuiValueSpace library transparently
    adds the 'Context.' prefix to all access functions.

    A call to:

    \code
    DuiValueSpaceItem topEdge("Context.Screen.TopEdge");
    \endcode

    ...is internally in \b DuiValueSpace converted to actual \c Screen.TopEdge
    wire access. This mechanism has been introduced to make the
    \b DuiValueSpace and \b libcontextsubscriber libraries parallel-installable.

    It's expected that all \b DuiValueSpace clients migrate to \b libcontextsubscriber
    eventually and \b DuiValueSpace library will be removed.

    \warning When migrating to \b libcontextsubscriber make sure to remove the 'Context.'
    from you key access paths.
*/

/*!
    \class ContextPropertyInfo

    \brief A class to introspect a context property details.

    This class is used to obtain information about a given key in the context registry.
    The information can be provided either from xml files or from a cdb database.
    It's possible to query the type, the provider and the documentation of the given
    key/property.
*/

/// Constructs a new ContextPropertyInfo for \a key with the given \a parent.
/// The object can be used to perform introspection on the given \a key.
/// \param key The full name of the key.
ContextPropertyInfo::ContextPropertyInfo(const QString &key, QObject *parent)
    : QObject(parent)
{
    keyName = key;

    if (key != "") {
        InfoBackend* infoBackend = InfoBackend::instance();
        sconnect(infoBackend, SIGNAL(keyDataChanged(QString)),
                 this, SLOT(onKeyDataChanged(QString)));

        cachedType = infoBackend->typeForKey(keyName);
        cachedDoc = InfoBackend::instance()->docForKey(keyName);
        cachedPlugin = infoBackend->pluginForKey(keyName);
        cachedConstructionString = infoBackend->constructionStringForKey(keyName);
    }
}

/// Returns the full name of the introspected key.
QString ContextPropertyInfo::key() const
{
    return keyName;
}

/// Returns the doc (documentation) for the introspected key.
QString ContextPropertyInfo::doc() const
{
    QReadLocker lock(&cacheLock);
    return cachedDoc;
}

/// Returns the type name for the introspected key.
QString ContextPropertyInfo::type() const
{
    QReadLocker lock(&cacheLock);
    return cachedType;
}

/// Returns true if the key exists in the registry.
bool ContextPropertyInfo::exists() const
{
    QReadLocker lock(&cacheLock);
    // A key is assumed to exist if it's type != "".
    if (cachedType != "")
        return true;
    else
        return false;
}

/// Returns the name of the plugin supplying this property
QString ContextPropertyInfo::plugin() const
{
    QReadLocker lock(&cacheLock);
    return cachedPlugin;
}

/// Returns the construction parameter for the Provider supplying this property
QString ContextPropertyInfo::constructionString() const
{
    QReadLocker lock(&cacheLock);
    return cachedConstructionString;
}

/// Returns the dbus name of the provider supplying this
/// property/key. This function is maintained for backwards
/// compatibility.
QString ContextPropertyInfo::providerDBusName() const
{
    QReadLocker lock(&cacheLock);
    // TBD: obsolete this function?
    if (cachedPlugin == "contextkit-dbus") {
        return cachedConstructionString.split(":").last();
    }
    return "";
}

/// Returns the bus type of the provider supplying this property/key.
/// Ie. if it's a session bus or a system bus. This function is
/// maintained for backwards compatibility.
QDBusConnection::BusType ContextPropertyInfo::providerDBusType() const
{
    QReadLocker lock(&cacheLock);
    // TBD: obsolete this function?
    QString busType = "";
    if (cachedPlugin == "contextkit-dbus") {
        busType = cachedConstructionString.split(":").first();
    }
    if (busType == "system")
        return QDBusConnection::SystemBus;
    else /* if (busType == "session") */
        return QDBusConnection::SessionBus;
}

/* Slots */

/// This slot is connected to the \a keyDataChanged signal of the
/// actual infobackend instance. It's executed on every change to any
/// of the keys. We first check if the data concerns us. Next we
/// update the cached values and fire the actual signals.
void ContextPropertyInfo::onKeyDataChanged(const QString& key)
{
    if (key != keyName)
        return;

    QWriteLocker lock(&cacheLock);

    QString newType = InfoBackend::instance()->typeForKey(keyName);
    if (cachedType != newType) {

        if (cachedType == "")
            emit existsChanged(true);
        if (newType == "")
            emit existsChanged(false);

        cachedType = newType;
        emit typeChanged(cachedType);
    }

    cachedDoc = InfoBackend::instance()->docForKey(keyName);

    // TBD: obsolete the providerChanged signal and add pluginChanged or sth?
    QString newPlugin = InfoBackend::instance()->pluginForKey(keyName);
    if (cachedPlugin == "contextkit-dbus" || newPlugin == "contextkit-dbus") {
        cachedPlugin = newPlugin;
        cachedConstructionString = InfoBackend::instance()->constructionStringForKey(keyName);
        QString newProvider = "";
        if (newPlugin == "contextkit-dbus") {
            newProvider = cachedConstructionString.split(":").last();
        }
        emit providerChanged(newProvider);
    }

}

