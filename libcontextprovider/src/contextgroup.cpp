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

#include "contextgroup.h"
#include "context.h"
#include "sconnect.h"
#include "logging.h"
#include "loggingfeatures.h"

using namespace ContextProvider;

/*! \class ContextGroup

    \brief Groups the firstSubscriberAppeared and
    lastSubscriberDisappeared from multiple Context objects together.

    ContextGroup is useful in cases when multiple properties are
    provided by the same sensor. When any of these properties is
    subscribed to, the sensor needs to be turned on, and when none of
    these properties are subscribed to, the sensor needs to be turned
    off.

    For example,
    \code
    Context* location("Location");
    Context* altitude("Altitude");

    QSet<Context*> gpsGroup;
    gpsGroup.insert(location);
    gpsGroup.insert(altitude);

    ContextGroup* gpsGrouper = new ContextGroup(gpsGroup);
    connect(gpsGrouper, SIGNAL(firstSubscriberAppeared()), this, SLOT(onGpsNeeded()));
    connect(gpsGrouper, SIGNAL(lastSubscriberDisappeared()), this, SLOT(onGpsNotNeeded()));

    void SomeClass::onGpsNeeded() {
        turnGpsOn();
    }

    void SomeClass::onGpsNotNeeded() {
        turnGpsOff();
    }
    \endcode

    This way, the provider doesn't need to store the subscription
    statuses of the related keys.

    Initially, none of the Context objects related to a ContextGroup
    are subscribed to. When some of them are subscribed to,
    ContextGroup emits the firstSubscriberAppeared signal. When all
    of them are again unsubscribed, ContextGroup emits the
    lastSubscriberDisappeared signal.

*/

/// Constructs a ContextGroup which watches the given set of Context objects.
ContextGroup::ContextGroup(QSet<Context*> propertiesToWatch, QObject* parent)
    : QObject(parent), propertiesSubscribedTo(0)
{
    contextDebug() << F_CONTEXTGROUP << "Creating new ContextGroup with" << propertiesToWatch.size() << "keys";
    
    foreach(Context* property, propertiesToWatch) {
        sconnect(property, SIGNAL(firstSubscriberAppeared(const QString&)),
                 this, SLOT(onFirstSubscriberAppeared()));
        sconnect(property, SIGNAL(lastSubscriberDisappeared(const QString&)),
                 this, SLOT(onLastSubscriberDisappeared()));
    }
}

/// Constructs a ContextGroup which listens to the given set of context keys
ContextGroup::ContextGroup(QStringList propertiesToWatch, QObject* parent)
    : QObject(parent), propertiesSubscribedTo(0)
{
    contextDebug() << F_CONTEXTGROUP << "Creating new ContextGroup with" << propertiesToWatch.size() << "keys";

    foreach(QString key, propertiesToWatch) {
        Context* property = new Context(key, this);
        sconnect(property, SIGNAL(firstSubscriberAppeared(const QString&)),
                 this, SLOT(onFirstSubscriberAppeared()));
        sconnect(property, SIGNAL(lastSubscriberDisappeared(const QString&)),
                 this, SLOT(onLastSubscriberDisappeared()));
    }
}

/// Called when any of the watched Context objects is subscribed to.
void ContextGroup::onFirstSubscriberAppeared()
{
    ++propertiesSubscribedTo;
    if (propertiesSubscribedTo == 1) {
        contextDebug() << F_CONTEXTGROUP << F_SIGNALS << "First subscriber appeared for group";
        emit firstSubscriberAppeared();
    }
}

/// Called when any of the watched Context objects is unsubscribed from.
void ContextGroup::onLastSubscriberDisappeared()
{
    --propertiesSubscribedTo;
    if (propertiesSubscribedTo == 0) {
        contextDebug() << F_CONTEXTGROUP << F_SIGNALS << "Last subscriber gone for group";
        emit lastSubscriberDisappeared();
    }
}

/// Returns true iff any Context objects in the group are subscribed to.
bool ContextGroup::isSubscribedTo() const
{
    return (propertiesSubscribedTo > 0);
}

/// Destructor
ContextGroup::~ContextGroup()
{
    contextDebug() << F_CONTEXTGROUP << F_DESTROY << "Destroying ContextGroup";
}
