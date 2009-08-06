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

namespace ContextProvider {

Listener::Listener(bool clears, ContextProviderSubscriptionChangedCallback cb, void *dt) 
    : callback(cb), user_data(dt), clearsOnSubscribe(clears)
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

PropertyListener::PropertyListener(Service &service, const QString &key,
                                   bool clears, ContextProviderSubscriptionChangedCallback cb, void *dt) 
    : Listener(clears, cb, dt), prop(service, key, this)
{
    sconnect(&prop, SIGNAL(firstSubscriberAppeared(QString)), this, SLOT(onFirstSubscriberAppeared()));
    sconnect(&prop, SIGNAL(lastSubscriberDisappeared(QString)), this, SLOT(onLastSubscriberDisappeared()));
}

void PropertyListener::clear()
{
    prop.unsetValue();
}

GroupListener::GroupListener(Service &service, const QStringList &keys,
                             bool clears, ContextProviderSubscriptionChangedCallback cb, void *dt)
    : Listener(clears, cb, dt), group(service, keys, this)
{
    sconnect(&group, SIGNAL(firstSubscriberAppeared()), this, SLOT(onFirstSubscriberAppeared()));
    sconnect(&group, SIGNAL(lastSubscriberDisappeared()), this, SLOT(onLastSubscriberDisappeared()));
}

void GroupListener::clear()
{
    foreach(Property *p, group.getProperties())
        p->unsetValue();
}

} // end namespace
