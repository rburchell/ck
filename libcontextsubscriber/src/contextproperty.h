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

    const ContextPropertyInfo* info() const;

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
