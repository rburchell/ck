#ifndef WATCHER_H
#define WATCHER_H
#include <QDebug>
#include <QDBusPendingCallWatcher>

class Watcher : public QDBusPendingCallWatcher
{
    Q_OBJECT;

public:
    Watcher(const QDBusPendingCall &call, QObject * parent = 0) :
        QDBusPendingCallWatcher(call, parent)
        {
            connect(this, SIGNAL(finished(QDBusPendingCallWatcher *)),
                    this, SLOT(deleteLater()));
        }
};
#endif
