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
#include "propertyadaptor.h"
#include "servicebackend.h"
#include "logging.h"
#include "sconnect.h"
#include "loggingfeatures.h"
#include "propertyprivate.h"

namespace ContextProvider {

/*!
    \class ServiceBackend ContextKit ContextKit

    \brief A ServiceBackend is the real worker behind Service.

    Multiple Service instances can share same ServiceBackend.
    The backend is the actual worker that operates on D-Bus
    and registers properties.

    The Service class actually proxies all methods to the ServiceBackend.
*/

QHash<QString, ServiceBackend*> ServiceBackend::instances;
ServiceBackend *ServiceBackend::defaultServiceBackend;

/// Creates new ServiceBackend with the given QDBusConnection. The
/// connection will be shared between Service and the provider
/// program, and the ServiceBackend will not register any service
/// names.
ServiceBackend::ServiceBackend(QDBusConnection connection) :
    connection(connection),
    refCount(0),
    busName("")  // shared connection
{
    contextDebug() << F_SERVICE_BACKEND << "Creating new ServiceBackend for" << busName;
}

/// Creates new ServiceBackend with the given QDBusConnection and a
/// service name to register. The connection will not be shared
/// between the Service and the provider program.
ServiceBackend::ServiceBackend(QDBusConnection connection, const QString &busName) :
    connection(connection),
    refCount(0),
    busName(busName)  // private connection
{
    contextDebug() << F_SERVICE_BACKEND << "Creating new ServiceBackend for" << busName;
}

/// Destroys the ServiceBackend. The backend is stopped.  If this
/// backend is the defaultServiceBackend, the defaultServiceBackend is
/// set back to NULL.
ServiceBackend::~ServiceBackend()
{
    contextDebug() << F_SERVICE_BACKEND << F_DESTROY << "Destroying Service";
    stop();

    if (properties.size() > 0) {
        contextCritical() << F_SERVICE_BACKEND << "Destroying a Service object "
                          << "before destroying the associated Property objects.";
    }

    if (ServiceBackend::defaultServiceBackend == this)
        ServiceBackend::defaultServiceBackend = 0;
}

/// Set the value of \a key to \a val.  A property named \a key must
/// have been registered already, by creating a Property object for
/// it.
void ServiceBackend::setValue(const QString &key, const QVariant &v)
{
    if (properties.contains(key))
        properties[key]->setValue(v);
    else
        contextWarning() << "Cannot set value for Property" << key << ", it does not exist";
}

/// Associate a PropertyPrivate object with this ServiceBackend. The
/// corresponding object will appear on D-Bus.
void ServiceBackend::addProperty(const QString& key, PropertyPrivate* property)
{
    properties.insert(key, property);
    contextDebug() << F_SERVICE << "registering property" << key;
    registerProperty(key, property);
}

// Break the association of the PropertyPrivate object with this
// ServiceBackend. The corresponding object will disappear from D-Bus.
void ServiceBackend::removeProperty(const QString& key)
{
    properties.remove(key);

    // Unregister the object on D-Bus
    PropertyAdaptor* adaptor = createdAdaptors[key];
    if (adaptor != 0) {
        connection.unregisterObject(adaptor->objectPath());
    }
}

/// Register a Property with the given name on D-Bus. Returns true if
/// succeeded, false if failed.
bool ServiceBackend::registerProperty(const QString& key, PropertyPrivate* property)
{
    if (createdAdaptors.contains(key) == false) {
        PropertyAdaptor* adaptor = new PropertyAdaptor(property, &connection);
        createdAdaptors.insert(key, adaptor);
    }
    PropertyAdaptor* adaptor = createdAdaptors[key];

    if (connection.objectRegisteredAt(adaptor->objectPath()) != 0) {
        // Object already registered; don't do anything
        return true;
    }

    if (!connection.registerObject(adaptor->objectPath(), property)) {
        contextCritical() << F_SERVICE_BACKEND << "Failed to register the Property object for" << key;
        contextCritical() << F_SERVICE_BACKEND << "Error:" << connection.lastError();
        return false;
    }
    return true;
}


/// Start the Service again after it has been stopped. In the case of
/// shared connection, the objects will be registered to D-Bus. In the
/// case of non-shared connection, also the service name will be
/// registered on D-Bus. Returns true on success, false otherwise.
bool ServiceBackend::start()
{
    contextDebug() << createdAdaptors;
    // Re-register existing Property objects on D-Bus
    foreach (const QString& key, properties.keys()) {
        if (!registerProperty(key, properties[key])) {
            return false;
        }
    }

    // Register the service name over D-Bus
    if (!sharedConnection()) {
        if (!connection.registerService(busName)) {
            contextCritical() << F_SERVICE_BACKEND << "Failed to register service with name" << busName;
            stop();
            return false;
        }
    }
    return true;
}

/// Stop the service. In the case of shared connection, this will
/// cause the related objects to be unregistered, but the bus name
/// will still be on D-Bus. In the case of non-shared connection, this
/// will cause the service to disappear from D-Bus completely.
void ServiceBackend::stop()
{
    contextDebug() << F_SERVICE_BACKEND << "Stopping service for bus:" << busName;

    // Unregister service name
    if (!sharedConnection())
        connection.unregisterService(busName);

    // Unregister Property objects and clean up PropertyAdaptors set
    foreach (PropertyAdaptor* adaptor, createdAdaptors) {
        connection.unregisterObject(adaptor->objectPath());
    }
}

/// Sets the ServiceBackend object as the default one to use when
/// constructing Property objects.
void ServiceBackend::setAsDefault()
{
    if (defaultServiceBackend) {
        contextCritical() << F_SERVICE_BACKEND << "Default service already set.";
        return;
    }

    defaultServiceBackend = this;
}

/// Increase the reference count by one. Service calls this.
void ServiceBackend::ref()
{
    refCount++;
}

/// Decrease the reference count by one. Service calls this. If the
/// reference count goes to zero, stop the ServiceBackend instance,
/// remove it from the instance store and schedule it to be deleted.
void ServiceBackend::unref()
{
    refCount--;

    if (refCount == 0) {
        stop();
        QString key = instances.key(this);
        if (key != "")
            instances.remove(key);
        else
            contextCritical() << "Backend couldn't find itself in the instance store";
        deleteLater(); // "delete this" would be probably unsafe
    }
}

/// Returns a ServiceBackend instance for a given \a
/// connection. Creates the instance if it does not exist yet.
ServiceBackend* ServiceBackend::instance(QDBusConnection connection)
{
    QString lookup = connection.name();
    contextDebug() << F_SERVICE << "Creating new Service for" << lookup;

    if (!instances.contains(lookup)) {
        ServiceBackend* backend = new ServiceBackend(connection);
        backend->start();
        instances.insert(lookup, backend);
    }
    return instances[lookup];
}

/// Returns a ServiceBackend instance for a given \a busType and \a
/// busName. Creates the instance if it does not exist yet.
ServiceBackend* ServiceBackend::instance(QDBusConnection::BusType busType,
                                         const QString &busName, bool autoStart)
{
    QString lookup = QString("contextprovider_") +
        ((busType == QDBusConnection::SessionBus) ? "session" : "system") +
        busName;
    contextDebug() << F_SERVICE << "Creating new Service for" << lookup;

    if (!instances.contains(lookup)) {
        ServiceBackend* backend = new ServiceBackend(
            QDBusConnection::connectToBus(busType, busName),
            busName);
        if (autoStart) backend->start();
        instances.insert(lookup, backend);
    }
    return instances[lookup];
}

/// Returns true iff the ServiceBackend shares its QDBusConnection
/// with the provider program.
bool ServiceBackend::sharedConnection()
{
    return busName == "";
}

} // end namespace
