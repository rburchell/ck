#ifndef SAFEDBUSPENDINGCALLWATCHER_H
#define SAFEDBUSPENDINGCALLWATCHER_H

#include "sconnect.h"
#include <QDBusPendingCallWatcher>

//! Deletes itself after the <tt>finished()</tt> signal is emitted
//! (and the mainloop entered).
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
