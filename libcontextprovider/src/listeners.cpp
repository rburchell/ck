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

Listener::Listener(bool clears, ContextProviderSubscriptionChangedCallback cb, void *dt) :
        callback(cb), user_data(dt), clearsOnSubscribe(clears)
{
}

void Listener::onFirstSubscriberAppeared()
{
    if (clearsOnSubscribe)
        clear();

    if (callback)
        callback(1, user_data);
}

void Listener::onLastSubscriberDisappeared()
{
    if (callback)
        callback(0, user_data);
}

ContextListener::ContextListener(const QString &k, bool clears, ContextProviderSubscriptionChangedCallback cb, void *dt) : 
        Listener(clears, cb, dt), key(k)
{
    sconnect(&key, SIGNAL(firstSubscriberAppeared(QString)), this, SLOT(onFirstSubscriberAppeared()));
    sconnect(&key, SIGNAL(lastSubscriberDisappeared(QString)), this, SLOT(onLastSubscriberDisappeared()));
}

void ContextListener::clear()
{
    key.unset();
}

ContextGroupListener::ContextGroupListener(const QStringList &keys, bool clears, ContextProviderSubscriptionChangedCallback cb, void *dt) : 
        Listener(clears, cb, dt), group(keys), keyList(keys)
{
    sconnect(&group, SIGNAL(firstSubscriberAppeared()), this, SLOT(onFirstSubscriberAppeared()));
    sconnect(&group, SIGNAL(lastSubscriberDisappeared()), this, SLOT(onLastSubscriberDisappeared()));
}

void ContextGroupListener::clear()
{
    foreach(QString k, keyList)
    {
        Context(k).unset();
    }
}

