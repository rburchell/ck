#ifndef TEST_H
#define TEST_H

#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QObject>
#include <QDebug>

class Test : public QObject
{
    Q_OBJECT;
public slots:
    void finished(QDBusPendingCallWatcher *watcher)
        {
            qDebug() << "finished";
            QDBusPendingReply<QDBusObjectPath> reply = *watcher;
            if (reply.isError()) {
                qDebug() << "error";
            } else {
                QDBusObjectPath path = reply.argumentAt<0>();
                qDebug() << path.path();
            }
        }
};
#endif
