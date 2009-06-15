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
#include "dbusnamelistener.h"

#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>
#include <QReadLocker>
#include <QWriteLocker>

namespace ContextSubscriber {

static const QDBusConnection::BusType commanderDBusType = QDBusConnection::SessionBus;
static const QString commanderDBusName = "org.freedesktop.ContextKit.Commander";

DBusNameListener* PropertyHandle::commanderListener = new DBusNameListener(commanderDBusType, commanderDBusName);
bool PropertyHandle::commandingEnabled = true;
bool PropertyHandle::typeCheckEnabled = false;

/*!
  \class PropertyHandle

  \brief A common handle for a context property.

  Only one handle exists at a time for a context property, no matter
  how much ContextProperty objects are created for it.

  Communication with the provider is done through the \c myProvider \c
  PropertyProvider instance, which is updated when needed because of
  registry changes.  Handling of disappearance from the DBus and then
  reappearance on the DBus of the same provider is handled privately
  by \c PropertyProvider.  If we don't know the current provider for
  this handle, then the \c myProvider pointer is 0.

  PropertyHandle and PropertyProvider instances are never deleted;
  they stick around until the process is terminated.
*/

PropertyHandle::PropertyHandle(const QString& key)
    :  myProvider(0), myInfo(0), subscribeCount(0), subscribePending(false), myKey(key)
{
    myInfo = new ContextPropertyInfo(myKey, this);

    updateProvider();

    // Start listening to changes in property registry (e.g., new keys, keys removed)
    sconnect(ContextRegistryInfo::instance(), SIGNAL(keysChanged(const QStringList&)),
             this, SLOT(updateProvider()));

    // Start listening for the beloved commander
    sconnect(commanderListener, SIGNAL(nameAppeared()),
             this, SLOT(updateProvider()));
    sconnect(commanderListener, SIGNAL(nameDisappeared()),
             this, SLOT(updateProvider()));

    // Move the PropertyHandle (and all children) to main thread.
    moveToThread(QCoreApplication::instance()->thread());
}

void PropertyHandle::ignoreCommander()
{
    commandingEnabled = false;
}

void PropertyHandle::setTypeCheck(bool typeCheck)
{
    typeCheckEnabled = typeCheck;
}

/// Decides who is the current provider of this property and sets up
/// \c myProvider accordingly.  If the provider has changed then
/// renews the subscriptions.
void PropertyHandle::updateProvider()
{
    PropertyProvider *newProvider;

    if (commandingEnabled && commanderListener->isServicePresent()) {
        // If commander is present it should be able to override the
        // property, so connect to it.
        newProvider = PropertyProvider::instance(commanderDBusType,
                                                 commanderDBusName);
    } else {
        // The myInfo object doesn't have to be re-created, because it
        // just routes the function calls to a registry backend.
        QString dbusName = myInfo->providerDBusName();

        if (dbusName != "") {
            // If myInfo knows the current provider which should be
            // connected to, connect to it.
            newProvider = PropertyProvider::instance(myInfo->providerDBusType(),
                                                     dbusName);
        } else {
            // Otherwise we keep the pointer to the old provider.
            // This way, we can still continue communicating with the
            // provider even though the key is no longer in the
            // registry.
            newProvider = myProvider;
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

/// Increase the \c subscribeCount of this context property and
/// subscribe to it through the \c myProvider instance if neccessary.
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

/// Decrease the \c subscribeCount of this context property and
/// unsubscribe from it through the \c myProvider instance if
/// neccessary.
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

/// Sets \c subscribePending to false if this property is contained in
/// the \c keys set.
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
    QReadLocker lock(&valueLock);
    return myValue;
}

bool PropertyHandle::isSubscribePending() const
{
    return subscribePending;
}

/// Used by the \c HandleSignalRouter to change the value of the
/// property.  Before changing the value it checks the type if type
/// checks are enabled.  It also verifies that the new value is
/// different from the old one if \c allowSameValue is false.  These
/// verification errors are signalled on the stderr.  After the checks
/// it updates the value and emits the valueChanged() signal.
void PropertyHandle::setValue(QVariant newValue, bool allowSameValue)
{
    if (typeCheckEnabled // type checks enabled
        && !newValue.isNull() // variable is non-null
        && myInfo->type() != "") { // the type is found in the registry

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

    QWriteLocker lock(&valueLock);
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
    // Container for singletons
    static QMap<QString, PropertyHandle*> handleInstances;

    // Protect the handleInstances. Documentation of QMap doesn't tell
    // if it can be read concurrently, so, read-write locking might
    // not be enough.

    static QMutex handleInstancesLock;
    QMutexLocker locker(&handleInstancesLock);
    if (!handleInstances.contains(key)) {
        // The handle does not exist, so create it
        handleInstances.insert(key, new PropertyHandle(key));
    }

    return handleInstances[key];
}

} // end namespace
