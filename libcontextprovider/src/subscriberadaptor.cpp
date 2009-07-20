/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "subscriberadaptor.h"
#include "sconnect.h"

/*!
    \class SubscriberAdaptor
    \brief A DBus adaptor for implementing the org.freedesktop.ContextKit.Subscriber
    interface. Using the Subscriber the remote client can subscribe and unsubscribe 
    from keys.
*/

/// Constructor. Creates new adaptor for the given \a subscriber Subscriber object.
SubscriberAdaptor::SubscriberAdaptor(Subscriber* subscriber)
    : QDBusAbstractAdaptor (subscriber), subscriber(subscriber)
{
    sconnect(subscriber, SIGNAL(changed(const QMap<QString, QVariant>&, const QStringList&)),
             this, SIGNAL(Changed(const QMap<QString, QVariant>&, const QStringList&)));
}

/// Performs the Subscriber::subscribe method to subscribe to a given set of keys.
QMap<QString, QVariant> SubscriberAdaptor::Subscribe(const QStringList &keys, QStringList &undetermined)
{
    return subscriber->subscribe(keys, undetermined);
}

/// Performs the Subscriber::unsubscribe method to unsubscribe from a given set of keys.
void SubscriberAdaptor::Unsubscribe(const QStringList &keys)
{
    return subscriber->unsubscribe(keys);
}

