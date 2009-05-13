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
#include "contextproperty_priv.h"
#include "propertyhandle.h"
#include "propertymanager.h"
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



void ContextProperty::init(const QString &key)
{
    priv = new ContextPropertyPrivate;

    priv->handle = PropertyManager::instance()->getHandle(key);
    priv->subscribed = false;

    subscribe();
}

void ContextProperty::finalize()
{
    unsubscribe ();
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
    PropertyManager::instance()->disableCommanding();
}

const ContextPropertyInfo& ContextProperty::info() const
{
    return priv->handle->info();
}
