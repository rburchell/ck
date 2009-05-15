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

#include "propertyhandle.h"
#include "propertyprovider.h"
#include "sconnect.h"
#include "contextpropertyinfo.h"
#include "contextregistryinfo.h"
#include "contextproperty.h"
#include "dbusnamelistener.h"

#include <QStringList>
#include <QString>
#include <QDebug>
#include <QPair>
#include <QMap>
#include <QDBusConnection>

QMap<QString, PropertyHandle*> PropertyHandle::handleInstances;

static const QDBusConnection::BusType commanderDBusType = QDBusConnection::SessionBus;
static const QString commanderDBusName = "org.freedesktop.ContextKit.Commander";
DBusNameListener* PropertyHandle::commanderListener = DBusNameListener::instance(commanderDBusType, commanderDBusName);

/*!
  \class PropertyHandle

  \brief A common handle for a context property.

  Only one handle exists at a time for a context property, no matter
  how much ContextProperty objects are created for it.

  Handling context properties and their providers.

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

/// Constructs a new instance. ContextProperty creates the handles, you
/// don't have to (and can't) call this constructor ever.
PropertyHandle::PropertyHandle(const QString& key)
    : myKey(key), myProvider(0), myInfo(0), subscribeCount(0), subscribePending(false)
{
    myInfo = new ContextPropertyInfo(myKey, this);

    onRegistryTouched();

    // Start listening to changes in property registry (e.g., new keys, keys removed)
    sconnect(ContextRegistryInfo::instance(), SIGNAL(keysChanged(QStringList)),
             this, SLOT(onRegistryTouched()));

    // Start listening for the beloved commander
    sconnect(commanderListener, SIGNAL(nameAppeared()),
             this, SLOT(onRegistryTouched()));
    sconnect(commanderListener, SIGNAL(nameDisappeared()),
             this, SLOT(onRegistryTouched()));
}

void PropertyHandle::onRegistryTouched()
{
    PropertyProvider *newProvider;

    if (commanderListener->isServicePresent()) {
        newProvider = PropertyProvider::instance(commanderDBusType,
                                                 commanderDBusName);
    } else {
        // The myInfo object doesn't have to be re-created, because it routes the function calls
        // to a registry backend.

        newProvider = myProvider;
        QString dbusName = myInfo->providerDBusName();

        // If the property is no longer in the registry, we keep the pointer to the old provider.
        // This way, we can still continue communicating with the provider even though the key
        // is no longer in the registry.
        if (dbusName != "") {
            newProvider = PropertyProvider::instance(myInfo->providerDBusType(),
                                                     dbusName);
        }
    }

    if (newProvider != myProvider && subscribeCount > 0) {
        // The provider has changed and ContextProperty classes are subscribed to this handle.
        // Unsubscribe from the old provider
        if (myProvider) myProvider->unsubscribe(myKey);
        // And subscribe to the new provider
        if (newProvider) newProvider->subscribe(myKey);
    }
    myProvider = newProvider;
}


/// Increase the subscribeCount of this context property and subscribe to DBUS if neccessary.
void PropertyHandle::subscribe()
{
    qDebug() << "PropertyHandle::subscribe";

    ++subscribeCount;
    if (subscribeCount == 1 && myProvider) {
        subscribePending = true;
        sconnect(myProvider, SIGNAL(subscribeFinished(QSet<QString>)),
                 this, SLOT(onSubscribeFinished(QSet<QString>)));
        myProvider->subscribe(myKey);
    }
}

/// Decrease the subscribeCount of this context property and unsubscribe from DBUS if neccessary.
void PropertyHandle::unsubscribe()
{
    --subscribeCount;
    if (subscribeCount == 0 && myProvider != 0) {
        subscribePending = false;
        myProvider->unsubscribe(myKey);
        disconnect(myProvider, SIGNAL(subscribeFinished(QSet<QString>)),
                   this, SLOT(onSubscribeFinished(QSet<QString>)));
    }
}

void PropertyHandle::onSubscribeFinished(QSet<QString> keys)
{
    if (keys.contains(myKey)) {
        subscribePending = false;
    }
}

QString PropertyHandle::key() const
{
    return myKey;
}

QVariant PropertyHandle::value() const
{
    return myValue;
}

PropertyProvider* PropertyHandle::provider() const
{
    return myProvider;
}

bool PropertyHandle::isSubscribePending() const
{
    return subscribePending;
}

/// Changes the value of the property and emits the valueChanged signal.
void PropertyHandle::setValue(QVariant newValue, bool allowSameValue)
{
    // FIXME : Implement the type check here. Remember to check the types of non-nulls only.
    if (!newValue.isNull() && ContextProperty::isTypeCheck()) {
        bool checked = false;
        QString myType = myInfo->type();
        if (myType == "STRING") {
            checked = (QVariant::String == newValue.type());
        } else if (myType == "TRUTH") {
            checked = (QVariant::Bool == newValue.type());
        } else if (myType == "DOUBLE") {
            checked = (QVariant::Double == newValue.type());
        } else if (myType == "INT") {
            checked = (QVariant::Int == newValue.type());
        }

        if (!checked) {
            qCritical() << "(PROVIDER) ERROR: bad type for" << myKey <<
                "wanted:" << myType << "got:" << newValue.typeName();
            return;
        }
    }

    // Note: the null we receive is always a non-typed null. We might need to convert it here.
    if (myValue == newValue && myValue.isNull() == newValue.isNull()) {
        // The property already has the value we received.
        // If we're processing the return values of Subscribe, this is normal,
        // since the return message contains values for the keys subscribed to.

        // If we're not processing the return values of Subscribe, it is an error.
        // Print out a message of provider error.
        // In either case, don't emit valueChanged.
        if (!allowSameValue) {
            qWarning() << "PROVIDER ERROR: Received unnecessary DBUS signal for property" << myKey;
        }
    } else {
        // The value was new
        myValue = newValue;
        emit valueChanged();
    }
}

const ContextPropertyInfo* PropertyHandle::info() const
{
    return myInfo;
}

PropertyHandle* PropertyHandle::instance(const QString& key)
{
    if (!handleInstances.contains(key))
        handleInstances.insert(key,
                               new PropertyHandle(key));

    Q_ASSERT(handleInstances[key] != 0);

    return handleInstances[key];
}
