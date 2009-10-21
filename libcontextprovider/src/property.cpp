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

#include "property.h"
#include "propertyprivate.h"
#include "service.h"
#include "servicebackend.h"
#include "logging.h"
#include "sconnect.h"
#include "loggingfeatures.h"
#include <QPair>

namespace ContextProvider {

/*!
    \class Property ContextProvider ContextProvider

    \brief A Property object represents a context property, i.e., a
    key-value pair.

    Each Property object is associated with a Service object at
    construction time.
*/

/// Create a Property object on \a service for the key \a k.
Property::Property(Service &service, const QString &k, QObject* parent)
    : QObject(parent)
{
    init(service.backend, k);
}

/// Create a Property object on the default service for the key \a k.
/// The default service can be set with Service::setAsDefault().
Property::Property(const QString &k, QObject* parent)
    : QObject(parent)
{
    if (ServiceBackend::defaultServiceBackend == NULL) {
        contextCritical() << "No default service set.";
        abort();
    }

    init(ServiceBackend::defaultServiceBackend, k);
}

void Property::init(ServiceBackend *serviceBackend, const QString &key)
{
    contextDebug() << F_PROPERTY << "Creating new Property for key:" << key;

    QPair<ServiceBackend*, QString> lookup(serviceBackend, key);

    if (PropertyPrivate::propertyPrivateMap.contains(lookup)) {
        priv = PropertyPrivate::propertyPrivateMap[lookup];
    }
    else {
        priv = new PropertyPrivate(serviceBackend, key);
        priv->serviceBackend->addProperty(priv->key, priv);
        PropertyPrivate::propertyPrivateMap.insert(lookup, priv);
    }
    sconnect(priv, SIGNAL(firstSubscriberAppeared(const QString&)),
             this, SIGNAL(firstSubscriberAppeared(const QString&)));
    sconnect(priv, SIGNAL(lastSubscriberDisappeared(const QString&)),
             this, SIGNAL(lastSubscriberDisappeared(const QString&)));
}

/// Returns true if the key is set (it's value is determined).
bool Property::isSet() const
{
    return (!priv->value.isNull());
}

/// Returns the name of the key this Property represents.
QString Property::key() const
{
    return priv->key;
}

/// Unsets the value. This is equivalent to setting it to a null
/// QVariant.
void Property::unsetValue()
{
    priv->setValue(QVariant());
}

/// Sets the property value to QVariant \a v.
void Property::setValue(const QVariant &v)
{
    priv->setValue(v);
}

/// Returns the current value of the property. The returned QVariant is invalid
/// if the key value is undetermined or the Property is invalid.
QVariant Property::value()
{
    return priv->value;
}

/// Destructor.
Property::~Property()
{
    contextDebug() << F_PROPERTY << F_DESTROY << "Destroying Property for key:" << priv->key;
}

} // end namespace
