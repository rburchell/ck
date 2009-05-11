#ifndef MANAGERINTERFACE_H
#define MANAGERINTERFACE_H

#include <QObject>
#include <QString>
#include <QtDBus>

/*
 * Proxy class for interface org.freedesktop.ContextKit.Manager
 */

class ManagerSignallingInterface;


class ManagerInterface : public QObject
{
    Q_OBJECT

    static const QString interfaceName;
    static const QString objectPath;

public:
    ManagerInterface(const QDBusConnection::BusType busType, const QString &busName, QObject *parent = 0);

    void getSubscriber();

private slots:
    void getSubscriberFinished(QDBusPendingCallWatcher* watcher);

signals:
    void getSubscriberFinished(QString objectPath);

private:
    QDBusInterface* iface;
};


#endif
