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

#include "contextproperty.h"
#include "contextproperty_priv.h"
#include "propertyhandle.h"
#include "keymanager.h"

///////////////////////////////////////////////////////////////////////////////
// define ContextProperty
///////////////////////////////////////////////////////////////////////////////

/*!
   \mainpage Context Properties

   \brief The Context Framework allows you to access system- and
   session-wide named values.  Examples are context properties like
   the current geographical location or settings like the current
   device language.  You can receive notifications about changes to
   these values, and you can also easily subscribe and unsubscribe
   from change notifications to help with managing power consumption.

   \section Overview

   The Context Properties are key/value pairs.  The keys are
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
   the key "Screen" is unrelated to "Screen.TopEdge".
   In particular, change notifications do not travel up the tree.

   You can get a list of all keys with the static
   ContextProperty::listKeys() member function.
   // FIXME: obsolete

   Programmers access the key/value pairs through instances of the
   ContextProperty class.  These instances allow applications to read
   item values and receive change notifications.

   Example:
   \code
   ContextProperty topEdge("Screen.TopEdge");
   QObject::connect(&topEdge, SIGNAL(valueChanged()),
                    this, SLOT(topEdgeChanged()));
   \endcode

   In your edgeUpChanged method you are able to get the data:
   \code
   qWarning() << "The edge " << topEdge.value() << " is up";
   \endcode

   Creating a ContextProperty instance for a key causes the program
   to 'subscribe' to this key.  The values for some keys might be
   expensive to determine, so you should only subscribe to those keys
   that you are currently interested in.  You can temporarily
   unsubscribe from a key without destroying the ContextProperty
   instance for it by using the unsubscribe() and subscribe() member
   functions.

   \code
   void onScreenBlank ()
   {
     topEdge.unsubscribe();
   }

   void onScreenUnblank ()
   {
     topEdge.subscribe();
   }
   \endcode

   The set of available keys in the Value Space can change at any time.
   Keys might disappear when their providers terminate or are
   uninstalled; new keys might appear when providers start or are
   installed; etc.

   Consequently, subscribing to a key that has no value associated with
   it is not an error.  Instead, the ContextProperty will return a
   'null' QVariant in that case.  If a provider for the key becomes
   available later, it will be used tranparently.

 */

/*!
   \class ContextPropertyPrivate

   \brief The private parts of the ContextProperty class.
*/

/*!
   \class ContextProperty

   \brief The ContextProperty class allows access to keys and their
   values in the Value Space.

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

   Instead, you can use the wait_for_subscription() member function.
   This function runs a recursive event loop, if necessary, until the
   ContextProperty is fully subscribed.

   Thus, the recommended way is to first create all ContextProperty
   instances that your program needs, then to call
   wait_for_subscription() on those values that are needed to create
   the initial user interface, and then to connect handlers for the
   relevant valueChanged() signals,

   It is important to create all ContextProperty instances before
   calling wait_for_subscription() on any of them.  Subscriptions are
   usually bundled together behind the scenes so that they can all be
   done with a single round trip to the provider.  Interleaving
   creation of ContextPropertys with calls to wait_for_subscription()
   would prevent this important optimization.

   \note
   The ContextProperty class is not thread safe and may only be used from
   an application's main thread
 */

/*!
   \fn ContextProperty::valueChanged()

   Emitted whenever the value of this item changes.
 */

/*!
   \fn ContextProperty::contentsChanged()

   Old, deprecated signal name for valueChanged(), will be removed.
 */

void ContextProperty::init(const QString &key)
{
    priv = new ContextPropertyPrivate;

    priv->pubInterface = this;
    priv->handle = KeyManager::getHandle(key);
    priv->subscribed = false;

    subscribe();
}

void ContextProperty::finalize()
{
    unsubscribe ();
    delete priv;
}

/*!

   Construct a new ContextProperty for \a key.

   \param key The full name of the key.  The empty string refers to the
   root of the key name hierarchy.
 */
ContextProperty::ContextProperty(const QString &key, QObject* parent)
    : QObject(parent), priv(0)
{
    init (key);
}

/*!

   Construct a new ContextProperty with the specified \a parent that
   refers to the same key as \a other.
 */
ContextProperty::ContextProperty(const ContextProperty &other, QObject* parent)
    : QObject(parent), priv(0)
{
    init (other.key());
}

ContextProperty::ContextProperty(QObject* parent)
    : QObject(parent), priv(0)
{
    init ("");
}

/*!
   Assign \a other to this.
 */
ContextProperty &ContextProperty::operator=(const ContextProperty& other)
{
    if (this == &other)
        return *this;

    finalize();
    init (other.key());

    return *this;
}

/*!
   Destroys the ContextProperty.
 */
ContextProperty::~ContextProperty()
{
    finalize();
}

/*!
   Returns the key of this ContextProperty.
 */
QString ContextProperty::key() const
{
    return priv->handle->key;
}

/*!

   Returns the current value of this item.
 */
QVariant ContextProperty::value() const
{
    return priv->handle->value;
}

/*!

  Sets the current value of this item.  Not all items can be modified.
  Contextual properties are read-only, for example.
 */
void ContextProperty::set(const QVariant &val)
{
    priv->handle->setValue(val);
}

/*!

   Returns the current value of this item.

   \param def Returned if the current value is \c null.
 */
QVariant ContextProperty::value(const QVariant &def) const
{
    QVariant val = priv->handle->value;
    if (val.isNull())
        return def;
    else
        return val;
}


/*!
   Returns the type of this item as defined in the registry.
 */
QString ContextProperty::type() const
{
    return priv->handle->typeName;
}


/*!
   Returns the description of this item.
 */
QString ContextProperty::description() const
{
    return priv->handle->description;
}
/*!

   Subscribe to the key of this item, if it isn't subscribed already.

   Subscription is not guaranteed to be complete immediately.  If you
   need to wait for it to be complete, use wait_for_subscription().
 */
void ContextProperty::subscribe() const
{
    if (priv->subscribed)
        return;

    QObject::connect(priv->handle, SIGNAL(handleChanged()),
                     this, SIGNAL(valueChanged()));
    QObject::connect(priv->handle, SIGNAL(handleChanged()),
                     this, SIGNAL(contentsChanged()));
    QObject::connect(priv->handle, SIGNAL(providerChanged()),
                     this, SIGNAL(providerChanged()));
    priv->handle->subscribe();
    priv->subscribed = true;
}

/*!

   Unsubscribe from the key of this item, if it isn't unsubcribed
   already.

   Unsubscribing informs the rest of the system that no effort needs to
   be spent to keep this item up-to-date, However, the item might still
   change when new values for it happen to be available 'for free'.
 */
void ContextProperty::unsubscribe() const
{
    if (!priv->subscribed)
        return;

    QObject::disconnect(priv->handle, SIGNAL(handleChanged()),
                        this, SIGNAL(contentsChanged()));
    QObject::disconnect(priv->handle, SIGNAL(handleChanged()),
                        this, SIGNAL(valueChanged()));
    QObject::disconnect(priv->handle, SIGNAL(providerChanged()),
                        this, SIGNAL(providerChanged()));
    priv->handle->unsubscribe();
    priv->subscribed = false;
}

/*!

   Wait until subcription is complete for this item.

   This might cause the main event loop of your program to run and
   consequently signal handlers and other callbacks might be invoked,
   including the valueChanged() handlers of this item.

   Calling this function while no subscription is in progress (because
   it has completed already or because the item is currently
   unsubscribed) does nothing.

 */
void ContextProperty::wait_for_subscription() const
{
    if (!priv->subscribed)
        return;

    while (!priv->handle->subscribed())
        QCoreApplication::processEvents();
}

/*!

   List the available context properties.

 */
QList<QString> ContextProperty::listKeys()
{
    return KeyManager::listKeys();
}

/*!

   This function should only be used by the Context Commander.

 */
void ContextProperty::iAmCommander()
{
    PropertyManager::instance()->disableCommanding();
}

/*!

   Get the dbus name and bus type of the provider of this property.

 */
QString ContextProperty::providerName() const
{
    return priv->handle->providerName();
}

