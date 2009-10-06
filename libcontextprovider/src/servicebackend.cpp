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

#include "servicebackend.h"
#include "logging.h"
#include "manager.h"
#include "manageradaptor.h"
#include "sconnect.h"
#include "loggingfeatures.h"

namespace ContextProvider {

/*!
    \class ServiceBackend ContextKit ContextKit

    \brief A ServiceBackend is the real worker behind Service.

    Multiple Service instances can share same ServiceBackend.
    The backend is the actual worker that operates on D-Bus,
    has a Manager and registers properties.

    The Service class actually proxies all methods to the ServiceBackend.
*/

QHash<QString, ServiceBackend*> ServiceBackend::instances;
ServiceBackend *ServiceBackend::defaultServiceBackend;

/// Creates new ServiceBackend with the given QDBusConnection. The
/// backend automatically creates it's Manager. The connection will be
/// shared between Service and the provider program, and the
/// ServiceBackend will not register any service names.
ServiceBackend::ServiceBackend(QDBusConnection connection) :
    connection(connection),
    refCount(0),
    busName("")  // shared connection
{
    contextDebug() << F_SERVICE_BACKEND << "Creating new ServiceBackend for" << busName;
}

/// Creates new ServiceBackend with the given QDBusConnection and a
/// service name to register. The backend automatically creates it's
/// Manager. The connection will not be shared between the Service and
/// the provider program.
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
    if (ServiceBackend::defaultServiceBackend == this)
        ServiceBackend::defaultServiceBackend = 0;
}

/// Returns the Manager associated with this backend.
Manager *ServiceBackend::manager()
{
    return &myManager;
}

/// Set the value of \a key to \a val.  A property named \a key must
/// have been registered already, by creating a Property object for
/// it.
void ServiceBackend::setValue(const QString &key, const QVariant &val)
{
    myManager.setKeyValue(key, val);
}

/// Start the Service again after it has been stopped. In the case of
/// shared connection, the objects will be registered to D-Bus. In the
/// case of non-shared connection, also the service name will be
/// registered on D-Bus. Returns true on success, false otherwise.
bool ServiceBackend::start()
{
    ManagerAdaptor *managerAdaptor = new ManagerAdaptor(&myManager, &connection);

    // Register object
    if (managerAdaptor && !connection.registerObject("/org/freedesktop/ContextKit/Manager", &myManager)) {
        contextCritical() << F_SERVICE_BACKEND << "Failed to register the Manager object for" << busName;
        contextCritical() << F_SERVICE_BACKEND << "Error:" << connection.lastError();
        return false;
    }

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
    // Unregister manager
    connection.unregisterObject("/org/freedesktop/ContextKit/Manager");
    // FIXME: unregister subscribers
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
