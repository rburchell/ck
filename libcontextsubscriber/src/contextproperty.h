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

///////////////////////////////////////////////////////////////////////////////
// define ContextProperty
///////////////////////////////////////////////////////////////////////////////

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

   Creating a ContextProperty instance for a key causes the program to
   'subscribe' to this key.  The values for some keys might be
   expensive to determine, so you should only subscribe to those keys
   that you are currently interested in.  You can temporarily
   unsubscribe from a key without destroying the ContextProperty
   instance for it by using the unsubscribe() member function. Later,
   you can resume the subscription by calling the and subscribe()
   member function.

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

   The set of available context properties can change at any time.
   Keys might disappear when their providers terminate or are
   uninstalled; new keys might appear when providers start or are
   installed; etc.

   Consequently, subscribing to a key that has no value associated
   with it is not an error.  Instead, the ContextProperty will return
   a 'null' QVariant in that case.  If a provider for the key becomes
   available later, the property will start receiving values from the
   provider transparently.

 */

#ifndef CONTEXTPROPERTY_H
#define CONTEXTPROPERTY_H



#include <QObject>
#include <QVariant>
#include <QString>

class ContextPropertyPrivate;
class ContextPropertyInfo;

/*!
   \class ContextPropertyPrivate

   \brief The private parts of the ContextProperty class.
*/

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

class ContextProperty : public QObject
{
    Q_OBJECT

public:
    explicit ContextProperty(QObject *parent = 0);
    explicit ContextProperty(const QString &key, QObject *parent = 0);

    virtual ~ContextProperty();

    QString key() const;
    QVariant value(const QVariant &def) const;
    QVariant value() const;

    ContextPropertyInfo info() const;

    void subscribe () const;
    void unsubscribe () const;

    void waitForSubscription () const;

    ContextProperty(const ContextProperty &other, QObject *parent = 0);
    ContextProperty &operator=(const ContextProperty&);

    // This function should only be called by the Commander.
    // It sets all the ContextProperty instances immune to commanding.
    static void ignoreCommander();

    static void setTypeCheck(bool typeCheck);

signals:
    void valueChanged();

private:
    ContextPropertyPrivate *priv;

    void init (const QString &key);
    void finalize ();

    friend class ContextPropertyPrivate;
};

#endif // CONTEXTPROPERTY_H
