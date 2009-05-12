#ifndef SAFEDBUSPENDINGCALLWATCHER_H
#define SAFEDBUSPENDINGCALLWATCHER_H

#include "sconnect.h"
#include <QDebug>
#include <QDBusPendingCallWatcher>

class SafeDBusPendingCallWatcher : public QDBusPendingCallWatcher
{
    Q_OBJECT;

public:
    SafeDBusPendingCallWatcher(const QDBusPendingCall &call, QObject * parent = 0) :
        QDBusPendingCallWatcher(call, parent)
        {
            sconnect(this, SIGNAL(finished(QDBusPendingCallWatcher *)),
                     this, SLOT(deleteLater()));
        }
};

#endif
