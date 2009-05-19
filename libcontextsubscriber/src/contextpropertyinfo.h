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
    \page Introspection

    \brief The introspection part of the API allows you to inspect the current
    state of the registry and observe the changes happening to it. 

    \section Overview

    The introspection is provided via two classes: ContextRegistryInfo and 
    ContextPropertyInfo. The first one is used to access the state of the registry itself. 
    You can use it to obtain info about the list of currently available keys in the registry 
    or ie. get a list of keys for one particular provider. ContextRegistryInfo provides a
    high-level view to the registry contents.

    ContextRegistryInfo is a singleton class that's created on the first access.

    The ContextPropertyInfo is used to obtain metadata about one particular key. Once created, 
    it can be used to get the type, provider and bus type of the introspected key. It also 
    provides a couple of useful signals for watching changes happening to a key.

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
    propInfo.provder();  //  ...returns empty string
    \endcode

    You can use this functionality to wait for keys to become availible in the registry. 
    Just create a ContextPropertyInfo for a key that you're expecting to become present
    and connect to the /c existsChanged signal.
    
    \code
    ContextPropertyInfo propInfo("Something.That.Doesnt.Exist");
    propInfo.exists(); // false
    // Connect something to the existsChanged signal.
    \endcode
    
    \section xmlvscdb XML vs.CDB
   
    When the introspection API is first used, a backend choice is being made. \b CDB backend
    (reading data from \c 'context-providers.cdb' ) is used if the tiny database cache file exists
    in the registry. The standard (slower) \b XML backend is used in other cases. 
   
    It's possible to force a usage of a particular backend. 
    This can be done by calling the \c instance method with a string name of the backend:
   
    \code
    ContextRegistryInfo::instance("cdb"); // or "xml"
    \endcode
   
    This needs to be done early enough before the introspection API is first used. 
    For more information about the \b xml and \cdb backends read the \ref "Updating context providers" page.

*/

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

    /// Destroys the object.
    virtual ~ContextPropertyInfo() {}

    QString key() const;
    QString doc() const;
    QString type() const;
    bool exists() const;
    
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

    /// Emitted when the key existance in the registry changes. The \a exists
    /// is the new state of the introspected key. This is a strict signal - it's 
    /// emitted only when there was an actual change in the state. Using this
    /// signal you can wait (watch) for various keys to become available.
    /// \param exists The new state of the key.
    void existsChanged(bool exists);
};

#endif // CONTEXTPROPERTYINFO_H
