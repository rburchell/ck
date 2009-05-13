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
#include "propertymanager.h"
#include "sconnect.h"
#include "contextpropertyinfo.h"

#include <QStringList>
#include <QHash>
#include <QString>
#include <QDebug>
#include <fcntl.h>
#include <string>

/*!
   \class PropertyHandle

   \brief A common handle for a context property.

   Only one handle exists at a time for a context property, no matter
   how much ContextProperty objects are created for it.
*/

/// Constructs a new instance. ContextProperty creates the handles, you
/// don't have to (and can't) call this constructor ever.
PropertyHandle::PropertyHandle(const QString& key)
    : myKey(key), myProvider(0), type(QVariant::Invalid), subscribeCount(0), subscribePending(false)
{
    update_provider();
}

/// Increase the subscribeCount of this context property and subscribe to DBUS if neccessary.
void PropertyHandle::subscribe()
{
    ++subscribeCount;
    if (subscribeCount == 1 && myProvider) {
        subscribePending = true;
        sconnect(myProvider, SIGNAL(subscribeFinished(QSet<QString>)),
                 this, SLOT(onSubscribeFinished(QSet<QString>)));
        myProvider->subscribe(this);
    }
}

/// Decrease the subscribeCount of this context property and unsubscribe from DBUS if neccessary.
void PropertyHandle::unsubscribe()
{
    --subscribeCount;
    if (subscribeCount == 0 && myProvider) {
        subscribePending = false;
        myProvider->unsubscribe(this);
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

/// Check the registry for the provider providing this property (named \c key) and
/// set up subscription and the \c provider pointer accordingly.
void PropertyHandle::update_provider()
{
    qDebug() << "PropertyHandle::update_provider";
    PropertyManager *manager = PropertyManager::instance();
    int provider_index;

    QVariant::Type new_type = QVariant::Invalid;
    PropertyProvider *new_provider = 0;

    if (manager->lookupProperty(myKey, provider_index, new_type, typeName, description))
        new_provider = manager->getProvider (provider_index);

    if (new_provider != myProvider) {
        qDebug() << "New provider:" << myKey;
        if (subscribeCount > 0) {
            if (myProvider)
                myProvider->unsubscribe(this);
            myProvider = new_provider;
            if (myProvider)
               myProvider->subscribe(this);
            // emit providerChanged();
        } else
            myProvider = new_provider;
    } else
        qDebug() << "Same provider:" << myKey;

    // XXX - what do we do when the type changes?

    if (new_type != type && type != QVariant::Invalid)
        qWarning() << "Type of" << myKey << "changed!";

    type = new_type;
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
void PropertyHandle::setValue(QVariant newValue)
{
    myValue = newValue;
    emit valueChanged();
}

ContextPropertyInfo PropertyHandle::info() const
{
    return ContextPropertyInfo(myKey);
}
