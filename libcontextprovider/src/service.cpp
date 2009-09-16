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
#include "manageradaptor.h"
#include "sconnect.h"
#include "loggingfeatures.h"

namespace ContextProvider {

/*!

  \mainpage Providing values for Context properties

  This library implements the provider side of the Context Framework.
  It has both a C++ and a C interface, so you can choose which you
  prefer. For the documentation of the C API, see \ref CApi.

  The C++ interface consists mainly of the three classes Service,
  Property, and Group in the namespace ContextProvider.  They are
  declared in the ContextProvider header file.

  Thus, you would typically gain access to the classes like this

  \code
  #include <ContextProvider>

  using namespace ContextProvider;

  Service my_service (...);
  \endcode

  If you prefer not to have generic names like "Service" in your code,
  you can of course skip the "using" declaration and refer to the
  classes as "ContextProvider::Service", etc.  If that is too long,
  consider a namespace alias like this:

  \code
  #include <ContextProvider>

  namespace CP = ContextProvider;

  CP::Service my_service (...);
  \endcode

  The basic pattern to use this library is to create a Service
  instance to represent you on D-Bus and then to add Property
  instances to it for the keys that you want to provide.  Once this is
  done, you can call Property::set() at any time to change the value
  of the property.

  Communication with clients happens asynchronously and this library
  needs a running event loop for that.

  Thus, a simple provider might look like this:

  \code
  #include <ContextProvider>

  using namespace ContextProvider;

  void main (int argc, char **argv)
  {
      QApplication app (argc, argv);

      Service my_service (QDBusConnection::SessionBus, "com.example.simple");
      Property my_property (my_service, "Example.Simple");

      // set initial value of property
      my_property.set (100);

      app.exec();
  }
  \endcode

  If you need to know when someone is actually subscribes to one of
  your values, you can connect to the firstSubscriberAppeared and
  lastSubscriberDisappeared signals of the Property instances.  You
  can also use a Group if you are only interested in whether at least
  one of a set of Property objects is subscribed to.

*/

/*!
    \class Service ContextKit ContextKit

    \brief A Service object is a proxy representing a service on D-Bus that
    implements the Context Framework interface. The lifespan of the
    Service class is not tied to the real service lifespan. This way
    a service can be accessed and controled from different parts of
    the code.

    When you first create a Service class with given session/name params
    the real dbus service instance is actually created and started.
    Future references to this session/name will give you a Service instance
    that points to the very same service as the first Service initialization
    that started it. In other words, we can say that the Service class represents
    a proxy interface to the real dbus service.

    When the last instance of Service is destroyed, the real service is automatically
    terminated and destroyed (there is a simple ref counting mechanism involved to
    guarantee that).

    Consider the following examples:

    \code
    Service *s1 = new Service("QDBusConnection::SessionBus, "com.example.simple");
    Service *s2 = new Service("QDBusConnection::SessionBus, "com.example.simple");
    // s1 and s2 represent same service

    s1->start();
    // Bots s1 and s2 are now started

    s2->start();
    // Bots s1 and s2 are now stopped

    delete s1; // s2 is still valid, service is present
    delete s2; // the "com.example.simple" just disappeared from
    \endcode

    Every Property object must be associated with a Service object.

    A Service can be running or stopped.  When it is running, it is
    visible via D-Bus and clients can subscribe to its properties.

    It is undefined what happens when you add a Property object to a
    running service.  Likewise, it is undefined what happens when you
    add a Property to a Group while the Service of that Property
    object is running.

    Thus, it is best to create a Service object, add all Property
    objects to it immediately, create the Group objects for them, and
    then enter the main loop, which will cause the Service to start.

*/

QHash <QString, ServiceBackend*> Service::backends;

struct ServicePrivate {
    ServiceBackend *backend;
    QString keystring;
};

/// Creates a Service proxy object for \a busName on the bus indicated by \a
/// busType. If the service is accessed for the first time it'll be created and
/// set up. If the service with the given parameters already exists the created
/// object represents a controller to a previously-created service.
/// A new Service is initially stopped and will automatically start itself
/// when the main loop is entered.
Service::Service(QDBusConnection::BusType busType, const QString &busName, QObject* parent)
    : QObject(parent)
{
    contextDebug() << F_SERVICE << "Creating new Service for" << busName;

    priv = new ServicePrivate;
    priv->keystring =  busName + ":" + ((busType == QDBusConnection::SessionBus) ? "Session" : "System");

    if (backends.contains(priv->keystring)) {
        priv->backend = backends.value(priv->keystring);
    } else {
        priv->backend = new ServiceBackend(busType, busName);
        backends.insert(priv->keystring, priv->backend);
    }

    priv->backend->ref();

    // XXX - there has be an easier way to get a idle callback when
    //       the event loop is entered.
    //
    connect(this, SIGNAL(startMe()),
            this, SLOT(onStartMe()),
            Qt::QueuedConnection);
    emit startMe();
}

/// Destroys this Service class. The actual service on dbus is destroyed and stopped
/// if this object is a last instance pointing at the actual service with the given
/// name and session params.
Service::~Service()
{
    contextDebug() << F_SERVICE << F_DESTROY << "Destroying Service";
    priv->backend->unref();

    if (priv->backend->refCount() == 0) {
        backends.remove(priv->keystring);
        delete priv->backend;
    }

    delete priv;
}

/// Returns the backend controller for this Service. Multiple Service classes
/// share one backend instance.
ServiceBackend *Service::backend()
{
    return priv->backend;
}

/// Sets the Service object as the default one to use when
/// constructing Property objects.
void Service::setAsDefault()
{
    priv->backend->setAsDefault();
}

/// Set the value of \a key to \a val.  A property named \a key must
/// have been registered already, by creating a Property object for
/// it.
void Service::setValue(const QString &key, const QVariant &val)
{
    priv->backend->manager()->setKeyValue(key, val);
}

/// Controls te service registration on dbus. If register service is set to
/// true (by default) the service while be registered on dbus. Set to false
//  if you want to reuse an existing service (ie. provided by piece of code).
void Service::setRegisterService(bool r)
{
    priv->backend->setRegisterService(r);
}

/// Start the Service again after it has been stopped.  All clients
/// will resubscribe to its properties. Returns true on success,
/// false otherwise.
bool Service::start()
{
    return priv->backend->start();
}

/// Stop the service.  This will cause it to disappear from D-Bus.
void Service::stop()
{
    priv->backend->stop();
}

/// If the service is running, stop and start it.
void Service::restart()
{
    priv->backend->restart();
}

/// Automatic startup when entering the run loop.
void Service::onStartMe()
{
    start();
}

} // end namespace
