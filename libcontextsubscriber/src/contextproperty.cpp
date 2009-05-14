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

#include "contextproperty.h"
#include "propertyhandle.h"
#include "sconnect.h"
#include "contextpropertyinfo.h"

#include <strings.h>
#include <QByteArray>
#include <QObject>
#include <QMap>
#include <QPair>
#include <QCoreApplication>
#include <QThread>
#include <QSet>
#include <QString>
#include <QVarLengthArray>

/*!
   \class ContextPropertyPrivate

   \brief The private parts of the ContextProperty class.
*/

bool ContextProperty::typeCheck = false;

struct ContextPropertyPrivate
{
    PropertyHandle *handle; ///< The common handle behind this context property
    bool subscribed; ///< True, if we are subscribed to the handle behind us
};

/*!
   \class ContextProperty

   \brief The ContextProperty class allows access to keys and their
   values.

   The value is available with the value() member function and change
   notifications are delivered via the valueChanged() signal.

   You can explicity subscribe and unsubscribe using the subscribe()
   and unsubscribe() member functions.  A ContextProperty is
   initially subscribed.

   When a ContextProperty is in the unsubscribed state, it usually
   keeps its last value and the valueChanged() signal is not
   emitted.  This is not guaranteed however: more than one
   ContextProperty might exist in your process for the same key, and
   as long as one of them is subscribed, all of them might receive new
   values.

   A ContextProperty is generally asynchronous and relies on a running
   event loop.  Subscritions and unsubcriptions are only handled and
   new values are only received when your program enters the event
   loop.

   When a ContextProperty is first created or goes from the
   unsubcribed to the subscribed state later on, it is temporarily in
   an intermediate 'subscribing' state.  This state lasts until the
   negotiations with the provider of the key are over and the key's
   current value is known to the ContextProperty.

   Thus, there is a time after creating a ContextProperty (or
   subscribing it again) where value() might be out of sync with the
   provider of the key.  If you need to wait for this time to be over,
   you can not rely on the valueChanged() signal being emitted.
   This signal is only emitted when the value actually changes, which
   might not happen when subscription is over.

   Instead, you can use the waitForSubscription() member function.
   This function runs a recursive event loop, if necessary, until the
   ContextProperty is fully subscribed.

   Thus, the recommended way is to first create all ContextProperty
   instances that your program needs, then to call
   waitForSubscription() on those values that are needed to create
   the initial user interface, and then to connect handlers for the
   relevant valueChanged() signals.
   // FIXME: Why not first connect handlers and then waitForSubscription?

   It is important to create all needed ContextProperty instances
   before calling waitForSubscription() on any of them.  Subscriptions
   are usually bundled together behind the scenes so that they can all
   be done with a single round trip to the provider.  Interleaving
   creation of ContextPropertys with calls to waitForSubscription()
   would prevent this optimization.

   \note
   The ContextProperty class is not thread safe and may only be used from
   an application's main thread.
 */

/*!
   \fn ContextProperty::valueChanged()

   Emitted whenever the value of the property changes.
 */

void ContextProperty::init(const QString &key)
{
    priv = new ContextPropertyPrivate;

    priv->handle = PropertyHandle::instance(key);
    priv->subscribed = false;

    subscribe();
}

void ContextProperty::finalize()
{
    unsubscribe();
    delete priv;
}

/// Constructs a new ContextProperty for \a key with the given \a parent.
/// \param key The full name of the key.
ContextProperty::ContextProperty(const QString &key, QObject* parent)
    : QObject(parent), priv(0)
{
    init (key);
}

/// Constructs a new ContextProperty with the given \a parent that
/// refers to the same key as \a other.
ContextProperty::ContextProperty(const ContextProperty &other, QObject* parent)
    : QObject(parent), priv(0)
{
    init (other.key());
}

/// Constructs a new ContextProperty with the given \a parent that
/// refers to the key "" (emtpy string).
// FIXME: Is this function needed?
ContextProperty::ContextProperty(QObject* parent)
    : QObject(parent), priv(0)
{
    init ("");
}

/// Assigns the ContextProperty \a other to this.
ContextProperty &ContextProperty::operator=(const ContextProperty& other)
{
    if (this == &other)
        return *this;

    finalize();
    init (other.key());

    return *this;
}

/// Destroys the ContextProperty.
ContextProperty::~ContextProperty()
{
    qDebug() << "ContextProperty::~ContextProperty";
    finalize();
}

/// Returns the key of the ContextProperty.
QString ContextProperty::key() const
{
    return priv->handle->key();
}

/// Returns the current value of the ContextProperty.
QVariant ContextProperty::value() const
{
    return priv->handle->value();
}

/// Returns the current value of this item, or the value \a def if the
/// current value is \c null.
QVariant ContextProperty::value(const QVariant &def) const
{
    QVariant val = priv->handle->value();
    if (val.isNull())
        return def;
    else
        return val;
}

/// Subscribes to the context property, if it isn't subscribed already.
/// Subscription is not guaranteed to be complete immediately.  If you
/// need to wait for it to be complete, use waitForSubscription().
void ContextProperty::subscribe() const
{
    if (priv->subscribed)
        return;

    sconnect(priv->handle, SIGNAL(valueChanged()),
             this, SIGNAL(valueChanged()));
    priv->handle->subscribe();
    priv->subscribed = true;
}

/// Unsubscribes from the context property, if it is currently
/// subscribed. Unsubscribing informs the rest of the system that no
/// effort needs to be spent to keep this item up-to-date, However,
/// the property might still change when new values for it happens to be
/// available 'for free'.
void ContextProperty::unsubscribe() const
{
    if (!priv->subscribed)
        return;

    QObject::disconnect(priv->handle, SIGNAL(valueChanged()),
                        this, SIGNAL(valueChanged()));
    priv->handle->unsubscribe();
    priv->subscribed = false;
}

/// Waits until subcription is complete for this context property.
/// This might cause the main event loop of your program to run and
/// consequently signal handlers and other callbacks might be invoked,
/// including the valueChanged() handlers of this item.
/// Calling this function while no subscription is in progress (because
/// it has completed already or because the item is currently
/// unsubscribed) does nothing.
void ContextProperty::waitForSubscription() const
{
    if (!priv->subscribed)
        return;

    while (priv->handle->isSubscribePending())
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
}

/// Sets the Context Property immune to 'commanding' (overriding
/// values done by Context Commander)
void ContextProperty::ignoreCommander()
{
// FIXME
//    PropertyManager::instance()->disableCommanding();
}

const ContextPropertyInfo* ContextProperty::info() const
{
    return priv->handle->info();
}

void ContextProperty::setTypeCheck(bool newTypeCheck)
{
    typeCheck = newTypeCheck;
}

bool ContextProperty::isTypeCheck()
{
    return typeCheck;
}
