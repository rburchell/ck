#ifndef QUEUEDINVOKER_H
#define QUEUEDINVOKER_H

#include <QObject>
#include <QMutex>
#include <QSet>
#include <QString>

class QueuedInvoker : public QObject
{
    Q_OBJECT

public:
    QueuedInvoker();

private Q_SLOTS:
    void onQueuedCall(const char *method);

Q_SIGNALS:
    void queuedCall(const char *method);

public:
    void queueOnce(const char *method);

private:
    QMutex callQueueLock;
    QSet<QString> callQueue;
};
#endif
