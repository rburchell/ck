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

#include "signalgrouper.h"
#include "context.h"
#include "sconnect.h"
#include "logging.h"
#include "loggingfeatures.h"

/*! \class SignalGrouper

    \brief Groups the firstSubscriberAppeared and
    lastSubscriberDisappeared from multiple Context objects together.

    SignalGrouper is useful in cases when multiple properties are
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

    SignalGrouper* gpsGrouper = new SignalGrouper(gpsGroup);
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

    Initially, none of the Context objects related to a SignalGrouper
    are subscribed to. When some of them are subscribed to,
    SignalGrouper emits the firstSubscriberAppeared signal. When all
    of them are again unsubscribed, SignalGrouper emits the
    lastSubscriberDisappeared signal.

*/

/// Constructs a SignalGrouper which watches the given set of Context objects.
SignalGrouper::SignalGrouper(QSet<Context*> propertiesToWatch, QObject* parent)
    : QObject(parent), propertiesSubscribedTo(0)
{
    contextDebug() << F_SIGNALGROUPER << "Creating new SignalGrouper with" << propertiesToWatch.size() << "keys";
    
    foreach(Context* property, propertiesToWatch) {
        sconnect(property, SIGNAL(firstSubscriberAppeared(const QString&)),
                 this, SLOT(onFirstSubscriberAppeared()));
        sconnect(property, SIGNAL(lastSubscriberDisappeared(const QString&)),
                 this, SLOT(onLastSubscriberDisappeared()));
    }
}

/// Constructs a SignalGrouper which listens to the given set of context keys
SignalGrouper::SignalGrouper(QStringList propertiesToWatch, QObject* parent)
    : QObject(parent), propertiesSubscribedTo(0)
{
    contextDebug() << F_SIGNALGROUPER << "Creating new SignalGrouper with" << propertiesToWatch.size() << "keys";

    foreach(QString key, propertiesToWatch) {
        Context* property = new Context(key, this);
        sconnect(property, SIGNAL(firstSubscriberAppeared(const QString&)),
                 this, SLOT(onFirstSubscriberAppeared()));
        sconnect(property, SIGNAL(lastSubscriberDisappeared(const QString&)),
                 this, SLOT(onLastSubscriberDisappeared()));
    }
}

/// Called when any of the watched Context objects is subscribed to.
void SignalGrouper::onFirstSubscriberAppeared()
{
    ++propertiesSubscribedTo;
    if (propertiesSubscribedTo == 1) {
        contextDebug() << F_SIGNALGROUPER << F_SIGNALS << "First subscriber appeared for group";
        emit firstSubscriberAppeared();
    }
}

/// Called when any of the watched Context objects is unsubscribed from.
void SignalGrouper::onLastSubscriberDisappeared()
{
    --propertiesSubscribedTo;
    if (propertiesSubscribedTo == 0) {
        contextDebug() << F_SIGNALGROUPER << F_SIGNALS << "Last subscriber gone for group";
        emit lastSubscriberDisappeared();
    }
}

/// Returns true iff any Context objects in the group are subscribed to.
bool SignalGrouper::isSubscribedTo() const
{
    return (propertiesSubscribedTo > 0);
}

/// Destructor
SignalGrouper::~SignalGrouper()
{
    contextDebug() << F_SIGNALGROUPER << F_DESTROY << "Destroying SignalGrouper";
}
