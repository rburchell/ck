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
    The backend is the actual worker that operates on dbus,
    has a Manager and registers properties.
*/

ServiceBackend *ServiceBackend::defaultService;

struct ServiceBackendPrivate {
    QDBusConnection::BusType busType;
    QString busName;
    Manager *manager;
    QDBusConnection *connection;
    int refCount;
};

/// Creates new ServiceBackend. The backend automatically creates it's Manager.
/// Connection is created on ServiceBackend::start().
ServiceBackend::ServiceBackend(QDBusConnection::BusType busType, const QString &busName, QObject* parent)
    : QObject(parent)
{
    contextDebug() << F_SERVICE_BACKEND << "Creating new ServiceBackend for" << busName;

    priv = new ServiceBackendPrivate;
    priv->busType = busType;
    priv->busName = busName;
    priv->manager = new Manager();
    priv->connection = NULL;
    priv->refCount = 0;
}

/// Destroys the ServiceBackend. The backend is not stopped.
ServiceBackend::~ServiceBackend()
{
    contextDebug() << F_SERVICE_BACKEND << F_DESTROY << "Destroying Service";
    stop();
    delete priv;
}

/// Returns the Manager associated with this backend.
Manager *ServiceBackend::manager()
{
    return priv->manager;
}

/// Set the value of \a key to \a val.  A property named \a key must
/// have been registered already, by creating a Property object for
/// it.
void ServiceBackend::setValue(const QString &key, const QVariant &val)
{
    priv->manager->setKeyValue(key, val);
}

/// Start the ServiceBackend again after it has been stopped.  All clients
/// will resubscribe to its properties. Returns true on success,
/// false otherwise.
bool ServiceBackend::start()
{
    if (priv->connection)
        return false;

    priv->connection = new QDBusConnection(QDBusConnection::connectToBus(priv->busType, priv->busName));
    ManagerAdaptor *managerAdaptor = new ManagerAdaptor(priv->manager, priv->connection);

    // Register service
    if (!priv->connection->registerService(priv->busName)) {
        contextCritical() << F_SERVICE_BACKEND << "Failed to register service with name" << priv->busName;
        stop();
        return false;
    }

    // Register object
    if (managerAdaptor && !priv->connection->registerObject("/org/freedesktop/ContextKit/Manager", priv->manager)) {
        contextCritical() << F_SERVICE_BACKEND << "Failed to register the Manager object for" << priv->busName;
        stop();
        return false;
    }

    return true;
}

/// Stop the service.  This will cause it to disappear from D-Bus.
void ServiceBackend::stop()
{
    if (priv->connection == NULL)
        return;

    contextDebug() << F_SERVICE_BACKEND << "Stopping service for bus:" << priv->busName;

    // Unregister
    priv->connection->unregisterObject("/org/freedesktop/ContextKit/Manager");
    priv->connection->unregisterService(priv->busName);

    // Dealloc
    delete priv->connection;
    priv->connection = NULL;
}

/// If the service is running, stop and start it.
void ServiceBackend::restart()
{
    if (priv->connection) {
        stop();
        start();
    }
}

/// Sets the ServiceBackend object as the default one to use when
/// constructing Property objects.  You can only set the default
/// Service once and the Service object that is the default must never
/// be deallocated.
void ServiceBackend::setAsDefault()
{
    if (defaultService) {
        contextCritical() << F_SERVICE_BACKEND << "Default service already set.";
        return;
    }

    defaultService = this;
}

/// Increase the reference count by one. Service calls that.
void ServiceBackend::ref()
{
    priv->refCount++;
}

/// Decrease the reference count by one. Service calls that.
void ServiceBackend::unref()
{
    priv->refCount--;
}

/// Return the current reference count. Service manages that.
int ServiceBackend::refCount()
{
    return priv->refCount;
}

} // end namespace
