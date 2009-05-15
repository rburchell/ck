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
#include "contextproperty.h"

#include <QStringList>
#include <QHash>
#include <QString>
#include <QDebug>
#include <fcntl.h>
#include <string>
#include <QPair>
#include <QMap>

QMap<QString, PropertyHandle*> PropertyHandle::handleInstances;

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

    QString dbusName = myInfo->providerDBusName();
    if (dbusName != "")
        myProvider = PropertyProvider::instance(myInfo->providerDBusType(),
                                                dbusName);
/* FIXME: this code is needed, if a handle's provider can change (registry can change)
    PropertyProvider *newProvider = 0;

    if (dbusName != "")
        newProvider = PropertyProvider::instance(myInfo->providerDBusType(),
                                   dbusName);

    if (newProvider != myProvider) {
        if (subscribeCount > 0) {
            if (myProvider)
                myProvider->unsubscribe(myKey);
            myProvider = newProvider;
            if (myProvider)
               myProvider->subscribe(myKey);
            // emit providerChanged();
        } else
            myProvider = newProvider;
    }
*/
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
