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
#include "provider.h"
#include "sconnect.h"
#include "contextpropertyinfo.h"
#include "contextregistryinfo.h"
#include "contextproviderinfo.h"
#include "dbusnamelistener.h"
#include "logging.h"
#include "loggingfeatures.h"

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
static const ContextProviderInfo commanderInfo("contextkit-dbus", "session:org.freedesktop.ContextKit.Commander");

DBusNameListener* PropertyHandle::commanderListener = new DBusNameListener(commanderDBusType, commanderDBusName);
bool PropertyHandle::commandingEnabled = true;
bool PropertyHandle::typeCheckEnabled = false;

/*!
  \class PropertyHandle

  \brief A common handle for a context property.

  Only one handle exists at a time for a context property, no matter
  how much ContextProperty objects are created for it.

  Communication with the provider is done through the \c myProvider \c
  Provider instance, which is updated when needed
  because of registry changes.  Handling of disappearance from the
  DBus and then reappearance on the DBus of the same provider is
  handled privately by \c Provider.  If we don't know
  the current provider for this handle, then the \c myProvider pointer
  is 0.

  PropertyHandle and Provider instances are never deleted;
  they stick around until the process is terminated.

  All of the PropertyHandle instances and Property provider instances
  are always created with the \c QCoreApplication's thread as the
  thread where they live.  This is needed, because user threads can go
  away and we would like to have only one DBus connection.
*/

PropertyHandle::PropertyHandle(const QString& key)
    :  myProvider(0), myInfo(0), subscribeCount(0), subscribePending(true), myKey(key)
{
    // Note: We set subscribePending to true, assuming that the
    // intention of the upper layer is to subscribe construction time.
    // If this intention is changed, changes are needed here as well.

    // Read the information about the provider. This needs to be
    // done before calling updateProvider.
    myInfo = new ContextPropertyInfo(myKey, this);

    // Start listening to changes in property introspection (e.g., added to registry, plugin changes)
    sconnect(myInfo, SIGNAL(changed(QString)),
             this, SLOT(updateProvider()));

    // Start listening for the context commander, and also initiate a
    // NameHasOwner check.

    sconnect(commanderListener, SIGNAL(nameAppeared()),
             this, SLOT(updateProvider()));
    sconnect(commanderListener, SIGNAL(nameDisappeared()),
             this, SLOT(updateProvider()));

    commanderListener->startListening(true);

    // Check if commander is already there:
    DBusNameListener::ServicePresence commanderPresence = commanderListener->isServicePresent();
    if (commanderPresence != DBusNameListener::Unknown) {
        // The status of the commander is known, so we can connect to
        // the provider (or commander) immediately.
        updateProvider();
    }
    // Otherwise, delay connecting to the provider until we know
    // whether commander is present.


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
    Provider *newProvider;
    contextDebug() << F_PLUGINS;

    if (commandingEnabled && commanderListener->isServicePresent() == DBusNameListener::Present) {
        // If commander is present it should be able to override the
        // property, so connect to it.
        newProvider = Provider::instance(commanderInfo);
    } else {
        // The myInfo object doesn't have to be re-created, because it
        // just routes the function calls to a registry backend.

        if (myInfo->provided()) {
            // If myInfo knows the current provider which should be
            // connected to, connect to it.
            contextDebug() << F_PLUGINS << "Key exists";
            QList<ContextProviderInfo> providers = myInfo->providers();
            if (providers.size() > 1)
                contextCritical() << "multi-process not implemented yet";
            else if (providers.size() == 0)
                contextCritical() << "property provided() but no providers() is empty";

            newProvider = Provider::instance(providers[0]);
        } else {
            // Otherwise we keep the pointer to the old provider.
            // This way, we can still continue communicating with the
            // provider even though the key is no longer in the
            // registry.
            contextDebug() << F_PLUGINS << "Key doesn't exist -> keep old provider info";

            newProvider = myProvider;

            if (newProvider == 0) {
                // This is the first place where we can know that the
                // provider for this property doesn't exist. We
                // shouldn't block waiting for subscription for such a property.
                subscribePending = false;
            }
        }
    }

    if (newProvider != myProvider && subscribeCount > 0) {
        // The provider has changed and ContextProperty classes are subscribed to this handle.
        // Unsubscribe from the old provider
        if (myProvider) myProvider->unsubscribe(myKey);
        // And subscribe to the new provider
        if (newProvider) subscribePending = newProvider->subscribe(myKey);
    }

    myProvider = newProvider;
}

/// Increase the \c subscribeCount of this context property and
/// subscribe to it through the \c myProvider instance if neccessary.
void PropertyHandle::subscribe()
{
    contextDebug() << F_THREADS << "PropertyHandle::subscribe" << QThread::currentThread();

    QMutexLocker locker(&subscribeCountLock);
    ++subscribeCount;
    if (subscribeCount == 1 && myProvider != 0) {
        subscribePending = myProvider->subscribe(myKey);
    }
}

/// Decrease the \c subscribeCount of this context property and
/// unsubscribe from it through the \c myProvider instance if
/// neccessary.
void PropertyHandle::unsubscribe()
{
    QMutexLocker locker(&subscribeCountLock);
    --subscribeCount;
    if (subscribeCount == 0 && myProvider != 0) {
        subscribePending = false;
        myProvider->unsubscribe(myKey);
    }
}

/// Sets \c subscribePending to false.
void PropertyHandle::setSubscribeFinished()
{
    subscribePending = false;
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
/// checks are enabled.  The verification errors are signalled on the
/// stderr.  After the check it updates the value and emits the
/// valueChanged() signal.
void PropertyHandle::onValueChanged()
{
    // FIXME: implement multiprocess here
    QVariant newValue = myProvider->get(myKey).value;

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
            contextCritical() << "Provider error, bad type for " << myKey <<
                "wanted:" << myType << "got:" << newValue.typeName();
            return;
        }
    }

    QWriteLocker lock(&valueLock);
    // Since QVariant(QVariant::Int) == QVariant(0), it's not enough to check
    // whether myValue and newValue are unequal.  Also, for completeness we
    // don't want to lose a valueChanged signal if the type changes.
    if (myValue != newValue ||
        myValue.isNull() != newValue.isNull() ||
        myValue.type() != newValue.type())
    {
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
