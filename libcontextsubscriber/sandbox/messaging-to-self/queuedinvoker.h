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

private slots:
    void onQueuedCall(const char *method);

signals:
    void queuedCall(const char *method);

public:
    void queueOnce(const char *method);

private:
    QMutex callQueueLock;
    QSet<QString> callQueue;
};
#endif
