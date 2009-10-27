#include "queuedinvoker.h"

#include <QMetaObject>
#include <QThread>
#include <QDebug>
#include <QMutexLocker>

QueuedInvoker::QueuedInvoker()
{
    connect(this, SIGNAL(queuedCall(const char *)),
            this, SLOT(onQueuedCall(const char *)),
            Qt::QueuedConnection);
}

void QueuedInvoker::onQueuedCall(const char *method)
{
    QMutexLocker locker(&callQueueLock);
    callQueue.remove(method);
    qDebug() << "Hope that i'm in the main loop" << QThread::currentThread();
    locker.unlock();
    if (!QMetaObject::invokeMethod(this, method, Qt::DirectConnection)) {
        qFatal("    *****************\n"
               "Erroneous usage of queueOnce(%s)\n"
               "    *****************\n", method);
    }
}

void QueuedInvoker::queueOnce(const char *method)
{
    QMutexLocker locker(&callQueueLock);
    if (!callQueue.contains(method)) {
        emit queuedCall(method);
        callQueue.insert(method);
    }
}
