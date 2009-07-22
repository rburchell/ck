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

#include "listeners.h"
#include "sconnect.h"

Listener::Listener(ContextProviderSubscriptionChangedCallback cb, void *dt) :
        callback(cb), user_data(dt)
{
}

void Listener::onFirstSubscriberAppeared()
{
    if (callback)
        callback(1, user_data);
}

void Listener::onLastSubscriberDisappeared()
{
    if (callback)
        callback(0, user_data);
}

ContextListener::ContextListener(const QString &k, ContextProviderSubscriptionChangedCallback cb, void *dt) : 
        Listener(cb, dt), key(k)
{
    sconnect(&key, SIGNAL(firstSubscriberAppeared()), this, SLOT(onFirstSubscriberAppeared));
    sconnect(&key, SIGNAL(lastSubscriberDisappeared()), this, SLOT(onLastSubscriberDisappeared));
}

ContextGroupListener::ContextGroupListener(const QStringList &keys, ContextProviderSubscriptionChangedCallback cb, void *dt) : 
        Listener(cb, dt), groupper(keys)
{
    sconnect(&groupper, SIGNAL(firstSubscriberAppeared()), this, SLOT(onFirstSubscriberAppeared));
    sconnect(&groupper, SIGNAL(lastSubscriberDisappeared()), this, SLOT(onLastSubscriberDisappeared));
}

