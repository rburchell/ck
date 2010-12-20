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

#include "contextkitplugin.h"
#include "logging.h"
#include "subscriberinterface.h"
#include "sconnect.h"
#include "propertyhandle.h"
#include <QStringList>
#include <QDBusPendingCall>
#include <QTimer>
#include <QDBusPendingReply>

/// Creates a new instance, the service to connect to has to be passed
/// in \c constructionString in the format <tt>[session|dbus]:servicename</tt>.
ContextSubscriber::IProviderPlugin* contextKitPluginFactory(QString constructionString)
{
    QStringList constr = constructionString.split(":");
    if (constr.size() != 2) {
        contextCritical() << "Bad syntax for contextkit-dbus plugin:" << constructionString;
        return 0;
    }
    if (constr[0] == "session") {
        ContextSubscriber::ContextKitPlugin* ret =
            new ContextSubscriber::ContextKitPlugin(QDBusConnection::sessionBus(), constr[1]);
        if (constructionString == ContextSubscriber::PropertyHandle::commanderInfo.constructionString)
            ret->setDefaultNewProtocol(false);
        return ret;
    } else if (constr[0] == "system")
        return new ContextSubscriber::ContextKitPlugin(QDBusConnection::systemBus(), constr[1]);

    contextCritical() << "Unknown bus type: " << constructionString;
    return 0;
}

namespace ContextSubscriber {

/*!
  \class ContextKitPlugin
  \brief Implementation of the ContextKit D-Bus protocol.
*/

static const char managerIName[] = "org.freedesktop.ContextKit.Manager";
static const char subscriberIName[] = "org.freedesktop.ContextKit.Subscriber";
static const char managerPath[] = "/org/freedesktop/ContextKit/Manager";
static const char propertyIName[] = "org.maemo.contextkit.Property";
static const char corePrefix[] = "/org/maemo/contextkit/";

/// Converts a key name to a protocol level object path.  There is a
/// distinction, because core properties have the form
/// <tt>/org/maemo/contextkit/Screen/TopEdge</tt> on D-Bus level, but
/// on higher levels they are <tt>Screen.TopEdge</tt>.  Non-core
/// properties should simply have a name like
/// /com/nokia/modem/Specific/Feature, so they can be used as object
/// paths without further conversions.
QString ContextKitPlugin::keyToPath(QString key)
{
    if (key.startsWith("/"))
        return key;

    return corePrefix + key.replace('.', '/').replace(QRegExp("[^A-Za-z0-9_/]"), "_");
}

/// Creates subscriber and manager interface, tries to get a
/// subscriber instance from the manager and starts listening for
/// provider appearing and disappearing on D-Bus.
ContextKitPlugin::ContextKitPlugin(const QDBusConnection bus, const QString& busName)
    : providerListener(new DBusNameListener(bus, busName, this)),
      subscriberInterface(0),
      managerInterface(0),
      connection(new QDBusConnection(bus)),
      busName(busName),
      newProtocol(true),
      defaultNewProtocol(true),
      providerAppearedQueued(false),
      providerAppearedSkip(false)
{
    reset();
    // Notice if the provider on the dbus comes and goes
    sconnect(providerListener, SIGNAL(nameAppeared()),
             this, SLOT(onProviderAppeared()));
    sconnect(providerListener, SIGNAL(nameDisappeared()),
             this, SLOT(onProviderDisappeared()));

    // Listen to the provider appearing and disappearing, but don't
    // perform the initial NameHasOwner check.  We try to connect to
    // the provider at startup, whether it's present or not.
    providerListener->startListening(false);
    QMetaObject::invokeMethod(this, "onProviderAppeared", Qt::QueuedConnection);
    providerAppearedQueued = true;
}

void ContextKitPlugin::setDefaultNewProtocol(bool s) {
    defaultNewProtocol = s;
}

void ContextKitPlugin::reset()
{
    delete(subscriberInterface);
    subscriberInterface = 0;
    delete(managerInterface);
    managerInterface = 0;
    newProtocol = defaultNewProtocol;
    // Disconnect the ValueChanged signal for all keys (object paths)
    connection->disconnect(busName, "", propertyIName, "ValueChanged",
                           this, SLOT(onNewValueChanged(QList<QVariant>,quint64,QDBusMessage)));
}

/// Gets a new subscriber interface from manager when the provider
/// appears.
void ContextKitPlugin::onProviderAppeared()
{
    // It is possible that this function is called and we have a Subscribe call
    // in progress.  This happens when things happen in the following order:
    // 1. the subscriber is started
    // 2. the subscriber optimistically sends the Subscribe call
    // 3. the provider is started (quick enough to handle the Subscribe call)
    // 4. providerListener notices that the provider was started
    // In this case, the plugin is in "ready" state already.

    providerAppearedQueued = false;

    if (providerAppearedSkip) {
        // Now we had queued this function to be called, but before it was
        // called, waitUntilReadyAndBlock called it.  Skip the scheduled call
        // (but not the later calls).
        providerAppearedSkip = false;
        return;
    }

    contextDebug() << "Provider appeared:" << busName;

    reset();
    if (defaultNewProtocol) {
        useNewProtocol();
        return;
    }
    managerInterface = new AsyncDBusInterface(busName, managerPath, managerIName, *connection, this);
    if (!managerInterface->callWithCallback("GetSubscriber",
                                            QList<QVariant>(),
                                            this,
                                            SLOT(onDBusGetSubscriberFinished(QDBusObjectPath)),
                                            SLOT(onDBusGetSubscriberFailed(QDBusError)))) {
        onDBusGetSubscriberFailed(managerInterface->lastError());
    }
}

/// Delete our subscriber interface when the provider goes away.
void ContextKitPlugin::onProviderDisappeared()
{
    contextDebug() << "Provider disappeared:" << busName;
    reset();
    Q_EMIT failed("Provider went away " + busName);
}

/// Starts using the fresh subscriber interface when it is returned by
/// the manager in response to the GetSubscriber call.
void ContextKitPlugin::onDBusGetSubscriberFinished(QDBusObjectPath objectPath)
{
    delete subscriberInterface;
    subscriberInterface = new SubscriberInterface(*connection, busName, objectPath.path(), this);
    sconnect(subscriberInterface,
             SIGNAL(valuesChanged(QMap<QString, QVariant>)),
             this,
             SLOT(onDBusValuesChanged(QMap<QString, QVariant>)));
    sconnect(subscriberInterface,
             SIGNAL(subscribeFinished(QList<QString>)),
             this,
             SLOT(onDBusSubscribeFinished(QList<QString>)));
    sconnect(subscriberInterface,
             SIGNAL(subscribeFailed(QList<QString>, QString)),
             this,
             SLOT(onDBusSubscribeFailed(QList<QString>, QString)));

    Q_EMIT ready();
}

void ContextKitPlugin::onDBusGetSubscriberFailed(QDBusError err)
{
    if (err.type() != QDBusError::UnknownObject) {
        contextWarning() <<
            "D-Bus failed for: " + busName + ", error: " +
            err.message();
        Q_EMIT failed("D-Bus error while trying old protocol " + busName);
        return;
    }
    contextWarning() <<
        "Trying new protocol, old failed for: " + busName + ", error: " +
        err.message();
    reset();
    useNewProtocol();
}

void ContextKitPlugin::useNewProtocol()
{
    newProtocol = true;

    if (providerListener->isServicePresent() == DBusNameListener::NotPresent)
        return;

    // Ready to try out new protocol. Ready should not be queued,
    // because otherwise ready and failed might get reordered.
    Q_EMIT ready();
}

/// Signals the Provider that the Subscribe call (old protocol) is finished.
void ContextKitPlugin::onDBusSubscribeFinished(QList<QString> keys)
{
    Q_FOREACH (const QString& key, keys)
        Q_EMIT subscribeFinished(key);
}

/// Signals the Provider that the Subscribe call (old protocol) has failed.
void ContextKitPlugin::onDBusSubscribeFailed(QList<QString> keys, QString error)
{
    Q_FOREACH (const QString& key, keys)
        Q_EMIT subscribeFailed(key, error);
}

/// Forwards the subscribe request to the wire.
void ContextKitPlugin::subscribe(QSet<QString> keys)
{
    if (newProtocol)
        Q_FOREACH (const QString& key, keys) {
            // Queue calling the Subscribe asynchronously. Don't
            // create the async call here: Qt will deadlock if we
            // create an async call while handling the results of the
            // previous async call. (We emit "ready" when handling
            // GetSubscriber. "Ready" is not queued, and the above
            // layer can call subscribe when handling it.)
            pendingKeys.insert(key);
            QMetaObject::invokeMethod(this, "newSubscribe", Qt::QueuedConnection, Q_ARG(QString, key));
        }
    else {
        subscriberInterface->subscribe(keys);
    }
}

void ContextKitPlugin::newSubscribe(const QString& key)
{
    if (pendingKeys.contains(key) == false) {
        // this key was already handled, probably because
        // waitForSubscriptionAndBlock forced the subscription to happen.
        return;
    }
    pendingKeys.remove(key);

    QString objectPath = keyToPath(key);
    // Store the "object path -> key" mapping so that we can transform
    // back when a valueChanged signal comes over D-Bus. (Note the
    // special character transformation.)
    objectPathToKey[objectPath] = key;

    QDBusPendingCall pc = connection->asyncCall(QDBusMessage::createMethodCall(busName,
                                                                               objectPath,
                                                                               propertyIName,
                                                                               "Subscribe"));

    // connect to dbus value changes too, but only for this key
    connection->connect(busName, objectPath, propertyIName, "ValueChanged",
                        this,
                        SLOT(onNewValueChanged(QList<QVariant>,quint64,QDBusMessage)));

    PendingSubscribeWatcher *psw = new PendingSubscribeWatcher(pc, key, this);
    pendingWatchers.insert(key, psw);
    sconnect(psw,
             SIGNAL(subscribeFinished(QString)),
             this,
             SIGNAL(subscribeFinished(QString)));
    sconnect(psw,
             SIGNAL(subscribeFailed(QString,QString)),
             this,
             SIGNAL(subscribeFailed(QString,QString)));
    sconnect(psw,
             SIGNAL(valueChanged(QString,TimedValue)),
             this,
             SIGNAL(valueChanged(QString,TimedValue)));
    sconnect(psw,
             SIGNAL(subscribeFinished(QString)),
             this,
             SLOT(removePendingWatcher(const QString&)));
    sconnect(psw,
             SIGNAL(subscribeFailed(QString,QString)),
             this,
             SLOT(removePendingWatcher(const QString&)));
}


/// Forwards the unsubscribe request to the wire.
void ContextKitPlugin::unsubscribe(QSet<QString> keys)
{
    if (newProtocol)
        Q_FOREACH (QString key, keys) {
            QString objectPath = keyToPath(key);
            objectPathToKey.remove(objectPath);
            connection->asyncCall(QDBusMessage::createMethodCall(busName,
                                                                 objectPath,
                                                                 propertyIName,
                                                                 "Unsubscribe"));
            // disconnect the ValueChanged signal for this key
            connection->disconnect(busName, objectPath,
                                   propertyIName, "ValueChanged",
                                   this,
                                   SLOT(onNewValueChanged(QList<QVariant>,quint64,QDBusMessage)));

        }
    else
        subscriberInterface->unsubscribe(keys);
}

/// Forwards value changes from the wire to the upper layer (Provider).
void ContextKitPlugin::onDBusValuesChanged(QMap<QString, QVariant> values)
{
    Q_FOREACH (const QString& key, values.keys())
        Q_EMIT valueChanged(key, values[key]);
}

// QDBus doesn't unmarshall non-basic types inside QVariants (since it cannot
// know the intention), but leaves them as QDBusArguments.  Here we walk the
// structure and unpack lists and maps.
QVariant demarshallValue(const QVariant &v)
{
    if (v.userType() != qMetaTypeId<QDBusArgument>())
        return v;
    const QDBusArgument &dba = v.value<QDBusArgument>();
    switch (dba.currentType()) {
    case QDBusArgument::ArrayType: {
        QVariantList vl;
        dba.beginArray();
        while (!dba.atEnd()) {
            QVariant v;
            dba >> v;
            vl << demarshallValue(v);
        }
        dba.endArray();
        return QVariant(vl);
        break;
    }
    case QDBusArgument::MapType: {
        dba.beginMap();
        QVariantMap vm;
        while (!dba.atEnd()) {
            QString k;
            QVariant v;
            dba.beginMapEntry();
            dba >> k >> v;
            dba.endMapEntry();
            v = demarshallValue(v);
            vm.insert(k, v);
        }
        dba.endMap();
        return QVariant(vm);
        break;
    }
    default:
        // Shouldn't reach this.
        contextWarning() << "got something unexpected: QDBusArgument of type"
                         << dba.currentType();
        return QVariant();
        break;
    }
}

static TimedValue createTimedValue(const QList<QVariant> value, quint64 time)
{
    if (value.size() == 0)
        return TimedValue(QVariant(), time);
    else
        return TimedValue(demarshallValue(value.at(0)), time);
}

void ContextKitPlugin::onNewValueChanged(QList<QVariant> value,
                                         quint64 timestamp,
                                         QDBusMessage message)
{
    if (objectPathToKey.contains(message.path())) {
        Q_EMIT valueChanged(objectPathToKey[message.path()],
                          createTimedValue(value, timestamp));
    }
    else {
        contextWarning() << "Unrecognized key" << message.path();
    }
}

void ContextKitPlugin::waitUntilReadyAndBlock()
{
    bool setSkip = providerAppearedQueued; // save this value
    onProviderAppeared();
    if (setSkip) {
        // Calling onProviderAppeared was already scheduled but we already
        // called it now.  Arrange so that the scheduled call is skipped when
        // the event is processed.
        providerAppearedSkip = true;
    }
}

void ContextKitPlugin::waitForSubscriptionAndBlock(const QString& key)
{
    // Force the subscriptions (that were scheduled) to happen now
    while (newProtocol && pendingKeys.size() > 0) {
        QString key = *(pendingKeys.constBegin());
        newSubscribe(key);
    }
    if (pendingWatchers.contains(key)) {
        pendingWatchers.value(key)->waitForFinished();
    }
}

void ContextKitPlugin::removePendingWatcher(const QString& key)
{
    pendingWatchers.remove(key);
}

PendingSubscribeWatcher::PendingSubscribeWatcher(const QDBusPendingCall &call,
                                                 const QString &key,
                                                 QObject * parent) :
    QDBusPendingCallWatcher(call, parent), key(key)
{
    sconnect(this, SIGNAL(finished(QDBusPendingCallWatcher *)),
             this, SLOT(onFinished()));
    sconnect(this, SIGNAL(finished(QDBusPendingCallWatcher *)),
             this, SLOT(deleteLater()));
}

void PendingSubscribeWatcher::onFinished()
{
    QDBusPendingReply<QList<QVariant>, quint64> reply = *this;
    if (reply.isError()) {
        Q_EMIT subscribeFailed(key, reply.error().message());
        return;
    }

    Q_EMIT valueChanged(key, createTimedValue(reply.argumentAt<0>(),
                                            reply.argumentAt<1>()));
    Q_EMIT subscribeFinished(key);
}

}
