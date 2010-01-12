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

#include "group.h"
#include "service.h"
#include "property.h"
#include "sconnect.h"
#include "logging.h"
#include "loggingfeatures.h"

namespace ContextProvider {

/*! \class Group ContextProvider ContextProvider

    \brief Groups the firstSubscriberAppeared and
    lastSubscriberDisappeared from multiple Property objects together.

    Group is useful in cases when multiple properties are provided by
    the same source, such as a hardware sensor.  When any of these
    properties is subscribed to, the source needs to be turned on, and
    when none of these properties are subscribed to, the source needs
    to be turned off.

    For example,
    \code
    Property location("Location");
    Property altitude("Altitude");

    Group gps;
    gps << location << altitude;

    connect(gps, SIGNAL(firstSubscriberAppeared()), this, SLOT(turnGpsOn()));
    connect(gps, SIGNAL(lastSubscriberDisappeared()), this, SLOT(onGpsOff()));
    \endcode

    This way, the provider doesn't need to store the subscription
    statuses of the related keys.

    Initially, none of the Property objects related to a Group
    are subscribed to. When some of them are subscribed to,
    Group emits the firstSubscriberAppeared signal. When all
    of them are again unsubscribed, Group emits the
    lastSubscriberDisappeared signal.

*/

struct GroupPrivate {
    int propertiesSubscribedTo;
    QSet<const Property *> properties;
};

/// Contructs an empty Group object with the given parent.
Group::Group(QObject* parent)
    : QObject(parent)
{
    contextDebug() << F_GROUP << "Creating new Group";

    priv = new GroupPrivate;
    priv->propertiesSubscribedTo = 0;
}

/// Adds a Property object to the Group. The Property object needs to
/// exist as long as the Group object is in use.
void Group::add(const Property &property)
{
    contextDebug() << F_GROUP << "Adding property" << property.key();

    priv->properties << &property;
    sconnect(&property, SIGNAL(firstSubscriberAppeared(const QString&)),
             this, SLOT(onFirstSubscriberAppeared()));
    sconnect(&property, SIGNAL(lastSubscriberDisappeared(const QString&)),
             this, SLOT(onLastSubscriberDisappeared()));
}

/// Returns the set of properties currently belonging to the Group.
QSet<const Property *> Group::getProperties()
{
    return priv->properties;
}

/// Called when some of the watched Property objects are subscribed to.
void Group::onFirstSubscriberAppeared()
{
    ++(priv->propertiesSubscribedTo);
    if (priv->propertiesSubscribedTo == 1) {
        contextDebug() << F_GROUP << F_SIGNALS << "First subscriber appeared for group";
        Q_EMIT firstSubscriberAppeared();
    }
}

/// Called when all of the watched Property objects are unsubscribed from.
void Group::onLastSubscriberDisappeared()
{
    --(priv->propertiesSubscribedTo);
    if (priv->propertiesSubscribedTo == 0) {
        contextDebug() << F_GROUP << F_SIGNALS << "Last subscriber gone for group";
        Q_EMIT lastSubscriberDisappeared();
    }
}

/// Returns true iff any Property objects in the group are subscribed to.
bool Group::isSubscribedTo() const
{
    return (priv->propertiesSubscribedTo > 0);
}

/// Destructor
Group::~Group()
{
    contextDebug() << F_GROUP << F_DESTROY << "Destroying Group";
    delete priv;
}

} // end namespace
