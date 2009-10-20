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
#include "servicebackend.h"
#include "property.h"
#include "logging.h"
#include "manager.h"
#include "sconnect.h"
#include "loggingfeatures.h"


namespace ContextProvider {

/*!
    \class Property ContextProvider ContextProvider

    \brief A Property object represents a context property, i.e., a
    key-value pair.

    Each Property object is associated with a Service object at
    construction time.
*/

struct PropertyPrivate {
    ServiceBackend* serviceBackend;
    QString key;
    QVariant value;
};

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

    init (ServiceBackend::defaultServiceBackend, k);
}

void Property::init (ServiceBackend *serviceBackend, const QString &key)
{
    contextDebug() << F_PROPERTY << "Creating new Property for key:" << key;

    priv = new PropertyPrivate;
    priv->key = key;
    priv->serviceBackend = serviceBackend;

    priv->serviceBackend->addProperty (priv->key, this);
    /*sconnect(priv->manager, SIGNAL(firstSubscriberAppeared(const QString&)),
             this, SLOT(onManagerFirstSubscriberAppeared(const QString&)));
    sconnect(priv->manager, SIGNAL(lastSubscriberDisappeared(const QString&)),
             this, SLOT(onManagerLastSubscriberDisappeared(const QString&)));
    */
    // FIXME: where do these signals come from?
}

/// Returns true if the key is set (it's value is determined).
bool Property::isSet() const
{
    return true; // FIXME
    //return (priv->manager->getKeyValue(priv->key) != QVariant());
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
    //priv->manager->setKeyValue(priv->key, QVariant());
}

/// Sets the property value to QVariant \a v.
void Property::setValue(const QVariant &v)
{
    priv->value = 
    //priv->manager->setKeyValue(priv->key, v);
}

/// Returns the current value of the property. The returned QVariant is invalid
/// if the key value is undetermined or the Property is invalid.
QVariant Property::value()
{
    return priv->value;
}

/// Called by Manager when first subscriber appears. Delegated if
/// this concerns us.
void Property::onManagerFirstSubscriberAppeared(const QString &key)
{
    if (key == priv->key) {
        contextDebug() << F_SIGNALS << F_PROPERTY << "First subscriber appeared for key:" << key;
        emit firstSubscriberAppeared(key);
    }
}

/// Called by Manager when last subscriber disappears. Delegate if
/// this concerns us.
void Property::onManagerLastSubscriberDisappeared(const QString &key)
{
    if (key == priv->key) {
        contextDebug() << F_SIGNALS << F_PROPERTY << "Last subscriber disappeared for key:" << key;
        emit lastSubscriberDisappeared(key);
    }
}

/// Destructor.
Property::~Property()
{
    contextDebug() << F_PROPERTY << F_DESTROY << "Destroying Property for key:" << priv->key;
    delete priv;
}

} // end namespace
