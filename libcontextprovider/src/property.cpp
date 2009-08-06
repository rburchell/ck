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

#include "service.h"
#include "property.h"
#include "logging.h"
#include "manager.h"
#include "sconnect.h"
#include "loggingfeatures.h"

namespace ContextProvider {

/*!
    \class Property

    \brief A Property object represents a Context property.

    Each Property object is associated with a Service object at
    construction time.
*/

/// Create a Property object on \a service for the key \a k.
Property::Property(Service &service, const QString &k, QObject* parent)
    : QObject(parent), myKey(k)
{
    init (&service);
}

/// Create a Property object on the default service for the key \a k.
/// The default service can be set with Service::setAsDefault().
Property::Property(const QString &k, QObject* parent)
    : QObject(parent), myKey(k)
{
    if (Service::defaultService == NULL) {
        contextCritical() << "No default service set.";
        abort();
    }

    init (Service::defaultService);
}

void Property::init (Service *service)
{
    contextDebug() << F_PROPERTY << "Creating new Property for key:" << myKey;
    
    service->add(this);
    manager = service->manager;

    sconnect(manager, SIGNAL(firstSubscriberAppeared(const QString&)),
             this, SLOT(onManagerFirstSubscriberAppeared(const QString&)));
    sconnect(manager, SIGNAL(lastSubscriberDisappeared(const QString&)),
             this, SLOT(onManagerLastSubscriberDisappeared(const QString&)));
}

/// Returns true if the key is set (it's value is determined).
bool Property::isSet() const
{
    return (manager->getKeyValue(myKey) != QVariant());
}

/// Returns the name of the key this Property represents.
QString Property::key() const
{
    return myKey;
}

/// Unsets the value. This is equivalent to setting it to a null
/// QVariant.
void Property::unsetValue()
{
    manager->setKeyValue(myKey, QVariant());
}

/// Sets the property value to QVariant \a v.
void Property::setValue(const QVariant &v)
{
    manager->setKeyValue(myKey, v);
}

/// Returns the current value of the property. The returned QVariant is invalid 
/// if the key value is undetermined or the Property is invalid.
QVariant Property::value()
{
    return manager->getKeyValue(myKey);
}

/// Called by Manager when first subscriber appears. Delegated if 
/// this concerns us.
void Property::onManagerFirstSubscriberAppeared(const QString &key)
{
    if (key == myKey) {
        contextDebug() << F_SIGNALS << F_PROPERTY << "First subscriber appeared for key:" << key;
        emit firstSubscriberAppeared(key);
    }
}

/// Called by Manager when last subscriber disappears. Delegate if
/// this concerns us.
void Property::onManagerLastSubscriberDisappeared(const QString &key)
{
    if (key == myKey) {
        contextDebug() << F_SIGNALS << F_PROPERTY << "Last subscriber disappeared for key:" << key;
        emit lastSubscriberDisappeared(key);
    }
}

/// Destructor.
Property::~Property()
{
    contextDebug() << F_PROPERTY << F_DESTROY << "Destroying Property for key:" << myKey;
}

} // end namespace
