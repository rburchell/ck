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

/*!
    \mainpage Context Properties

    \brief The Context Framework allows you to access system- and
    session-wide named values. Examples are context properties like the
    current geographical location. You can receive notifications about
    changes to these values, and you can also easily subscribe and
    unsubscribe from change notifications to help with managing power
    consumption.

    \section Overview

    The Context Properties are key/value pairs. The keys are
    strings and the values are QVariants.

    Key are arranged in a hierarchical name space like in this example
    of two contextual properties

    \code
    Screen.TopEdge = "left"
    Screen.IsCovered = false
    \endcode

    Although the key names can be considered to form a tree (with
    "Screen" at the root in the preceeding example, etc.) there is no
    semantic relationship between parent and child nodes in that tree:
    the key "Screen" is unrelated to "Screen.TopEdge".  In particular,
    change notifications do not travel up the tree.

    The \ref Introspection section describes in detail how to get a list of
    existing keys and examine their capabilities.

    Programmers access the key/value pairs through instances of the
    ContextProperty class.  These instances allow applications to read
    item values and receive change notifications.

    Example:
    \code
    ContextProperty *topEdge = new ContextProperty("Screen.TopEdge");
    QObject::connect(topEdge, SIGNAL(valueChanged()),
                     this, SLOT(topEdgeChanged()));
    \endcode

    In your edgeUpChanged method you are able to get the data:
    \code
    qWarning() << "The edge " << topEdge->value() << " is up";
    \endcode

    Creating a ContextProperty instance for a key causes the program
    to 'subscribe' to this key.  The values for some keys might be
    expensive to determine, so you should only subscribe to those keys
    that you are currently interested in.  You can temporarily
    unsubscribe from a key without destroying the ContextProperty
    instance for it by using the unsubscribe() member function. Later,
    you can resume the subscription by calling the subscribe() member
    function.

    \code
    void onScreenBlank ()
    {
        topEdge->unsubscribe();
    }

    void onScreenUnblank ()
    {
        topEdge->subscribe();
    }
    \endcode

    All of the context properties can be used anytime, it doesn't
    matter if the provider of the property is running or not,
    installed or not.  If the system/provider can't provide you with a
    value, the value of the context property will be null.  If for
    some reason you are interested in property metadata (such as a
    key's current provider, availability, etc.) you should consult the
    \ref Introspection API.
*/

#include "contextproperty.h"
#include "propertyhandle.h"
#include "sconnect.h"

#include <QCoreApplication>
#include <QThread>

using namespace ContextSubscriber;

/*!
   \class ContextPropertyPrivate

   \brief The private parts of the ContextProperty class.
*/

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
   keeps its last value.  This is not guaranteed however: more than
   one ContextProperty might exist in your process for the same key,
   and as long as one of them is subscribed, all of them might receive
   new values.  The valueChanged() signal is never emitted if the
   property is unsubscribed.

   A ContextProperty is generally asynchronous and relies on a running
   event loop.  Subscritions and unsubcriptions are only handled and
   new values are only received when your program enters the event
   loop.

   When a ContextProperty is first created or goes from the
   unsubcribed to the subscribed state later on, it is temporarily in
   an intermediate 'subscribing' state.  This state lasts until the
   negotiations with the provider of the key are over (or an error
   occurs) and the key's current value is known to the
   ContextProperty.

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
   instances that your program needs and QObject::connect their
   valueChanged() signals, then to call waitForSubscription() on those values
   that are needed to create the initial user interface.

   It is important to create all needed ContextProperty instances
   before calling waitForSubscription() on any of them.  Subscriptions
   are usually bundled together behind the scenes so that they can all
   be done with a single round trip to the provider.  Interleaving
   creation of ContextPropertys with calls to waitForSubscription()
   would prevent this optimization.

   \note The \c ContextProperty class follows the usual QObject rules
   for non-GUI classes in multi-threaded programs.  In Qt terminology,
   the ContextProperty class is reentrant but not thread-safe.  This
   means that you can create ContextProperty instances in any thread
   and then freely use these instance in their threads, but you can
   not use a single instance concurrently from multiple threads.

   \note Please pay special attention to how signals and slots work in
   a multi-threaded program: by default, a slot is emitted in the
   thread that called QObject::connect().  For this to happen
   reliably, the thread needs to run a event loop.

   \note See the Qt documentation for \c QThread and related classes
   for more details.
 */

/// Constructs a new ContextProperty for \a key and subscribes to it.
ContextProperty::ContextProperty(const QString &key, QObject* parent)
    : QObject(parent), priv(0)
{
    priv = new ContextPropertyPrivate;

    priv->handle = PropertyHandle::instance(key);
    priv->subscribed = false;

    subscribe();
}

/// Unsubscribes from the ContextProperty and destroys it.
ContextProperty::~ContextProperty()
{
    qDebug() << "ContextProperty::~ContextProperty";
    unsubscribe();
    delete priv;
}

/// Returns the key.
QString ContextProperty::key() const
{
    return priv->handle->key();
}

/// Returns the current value.
QVariant ContextProperty::value() const
{
    return priv->handle->value();
}

/// Returns the current value, or the value \a def if the current
/// value is \c null.
QVariant ContextProperty::value(const QVariant &def) const
{
    QVariant val = priv->handle->value();
    if (val.isNull())
        return def;
    else
        return val;
}

/// Starts subscribtion to the context property, if it isn't
/// subscribed already. If you need to wait for it to be complete, use
/// waitForSubscription().
void ContextProperty::subscribe() const
{
    if (priv->subscribed)
        return;

    // We create a queued connection, because otherwise we run
    // the users' valueChanged() handlers with locks and if they do
    // something fancy (for example unsubscribe) it can cause a
    // deadlock.
    sconnect(priv->handle, SIGNAL(valueChanged()), this, SIGNAL(valueChanged()), Qt::QueuedConnection);
    priv->handle->subscribe();
    priv->subscribed = true;
}

/// Unsubscribes from the context property, if it is currently
/// subscribed. Unsubscribing informs the rest of the system that no
/// effort needs to be spent to keep the value up-to-date.  However,
/// the value might still change when it can happen 'for free'.  In
/// this case the valueChanged() signal won't be emitted.
void ContextProperty::unsubscribe() const
{
    if (!priv->subscribed)
        return;

    QObject::disconnect(priv->handle, SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));
    priv->handle->unsubscribe();
    priv->subscribed = false;
}

/// Suspends the execution of the current thread until subcription is
/// complete for this context property.  This might cause the main
/// event loop of your program to run and consequently signals might
/// get emitted (including the valueChanged() signal of this
/// property).  Calling this function while the subscription is not in
/// progress (because it has completed already or because the property
/// is currently unsubscribed) does nothing.
void ContextProperty::waitForSubscription() const
{
    if (!priv->subscribed)
        return;

    // This is not a busy loop, since the QEventLoop::WaitForMoreEvents flag
    while (priv->handle->isSubscribePending()) {
        if (QThread::currentThread() == QCoreApplication::instance()->thread()) {
            QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
        } else {
            usleep(100000); // 0.1 second
        }
    }
}

/// Sets all of the ContextProperty instances immune to 'external
/// commanding'.  This is only intended to be used by the Context
/// Commander itself, so that it can use ContextProperties without
/// tripping over itself.  Don't use this.
void ContextProperty::ignoreCommander()
{
    PropertyHandle::ignoreCommander();
}

/// Returns the metadata about this property, please refer to \ref
/// Introspection for details.
const ContextPropertyInfo* ContextProperty::info() const
{
    return priv->handle->info();
}

/// Enables or disables all of the ContextProperty instances'
/// type-check feature.  If it is enabled and the received value from
/// the provider doesn't match the expected type, you will get an
/// error message on the stderr and the value won't be updated. If you
/// use this method, you have to use it before starting any threads.
void ContextProperty::setTypeCheck(bool newTypeCheck)
{
    PropertyHandle::setTypeCheck(newTypeCheck);
}

