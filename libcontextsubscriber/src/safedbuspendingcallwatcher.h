#ifndef SAFEDBUSPENDINGCALLWATCHER_H
#define SAFEDBUSPENDINGCALLWATCHER_H

#include <QDebug>
#include <QDBusPendingCallWatcher>

class SafeDBusPendingCallWatcher : public QDBusPendingCallWatcher
{
    Q_OBJECT;

public:
    SafeDBusPendingCallWatcher(const QDBusPendingCall &call, QObject * parent = 0) :
        QDBusPendingCallWatcher(call, parent)
        {
            connect(this, SIGNAL(finished(QDBusPendingCallWatcher *)),
                    this, SLOT(deleteLater()));
        }
};

#endif
