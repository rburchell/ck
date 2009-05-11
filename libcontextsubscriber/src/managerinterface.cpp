
#include "managerinterface.h"

#include "safedbuspendingcallwatcher.h"

const QString ManagerInterface::interfaceName = "org.freedesktop.ContextKit.Manager";
const QString ManagerInterface::objectPath = "/org/freedesktop/ContextKit/Manager";


ManagerInterface::ManagerInterface(const QDBusConnection::BusType busType, const QString &busName, QObject *parent)
    : iface(0)
{
    QDBusConnection connection("");

    // Create DBus connection
    if (busType == QDBusConnection::SessionBus) {
        connection = QDBusConnection::sessionBus();
    } else if (busType == QDBusConnection::SystemBus) {
        connection = QDBusConnection::systemBus();
    } else {
        qCritical() << "Invalid bus type: " << busType;
        return;
    }

    if (!connection.isConnected()) {
        qCritical() << "Couldn't connect to DBUS: ";
        return;
    }
    iface = new QDBusInterface(busName, objectPath, interfaceName, connection, this);
}

void ManagerInterface::getSubscriber()
{
    if (iface == 0) {
        return;
    }

    QDBusPendingCall getSubscriberCall = iface->asyncCall("GetSubscriber");
    SafeDBusPendingCallWatcher *watcher = new SafeDBusPendingCallWatcher(getSubscriberCall, this);

    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
                     this, SLOT(getSubscriberFinished(QDBusPendingCallWatcher *)));
}

void ManagerInterface::getSubscriberFinished(QDBusPendingCallWatcher* watcher)
{
     QDBusPendingReply<QDBusObjectPath> reply = *watcher;
     if (reply.isError()) {
         // Possible causes of the error:
         // The provider is not running
         // The provider didn't implement the needed interface + function
         // The function resulted in an error
         qWarning() << "Provider error:" << reply.error().message();
     } else {
         QDBusObjectPath path = reply.argumentAt<0>();
         qDebug() << path.path();

         emit getSubscriberFinished(path.path());
     }
}

