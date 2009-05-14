#ifndef DBUSNAMELISTENER_H
#define DBUSNAMELISTENER_H

#include <QObject>
#include <QDBusConnection>
#include <QString>

class DBusNameListener : public QObject
{
    Q_OBJECT
public:
    DBusNameListener(const QDBusConnection::BusType busType, const QString &busName);

private slots:
    void onServiceOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);

private:
    QString busName;

signals:
    void nameAppeared();
    void nameDisappeared();
};

#endif
