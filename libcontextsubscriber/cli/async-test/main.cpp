#include "test.h"
#include "watcher.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QApplication>
#include <QDBusPendingCall>
#include <QDebug>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QDBusConnection conn(QDBusConnection::sessionBus());
    QDBusInterface iface("com.nokia.SensorD",
                         "/org/freedesktop/ContextKit/Manager",
                         "org.freedesktop.ContextKit.Manager",
                         conn, &app);

    QDBusPendingCall myCall = iface.asyncCall("GetSubscriber");
    Watcher *myWatcher = new Watcher(myCall, &app);
    Test test;

    QObject::connect(myWatcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
                     &test, SLOT(finished(QDBusPendingCallWatcher *)));

    return app.exec();
}
