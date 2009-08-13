/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "queuedinvoker.h"

#include <QMetaObject>
#include <QThread>
#include <QDebug>
#include <QMutexLocker>

namespace ContextProvider {

/*! \class QueuedInvoker
    \brief A class that can invoke its own methods in a delayed way.

    Via the method QueuedInvoker::queueOnce, the given method is set
    to be invoked when the event loop of the object is entered. Each
    method is queued at most once. QueuedInvoker sends a signal to
    itself, and when the signal is processed, the method is invoked.

    QueuedInvoker is normally used by subclassing it.

*/

QueuedInvoker::QueuedInvoker(QObject* parent)
    : QObject(parent)
{
    connect(this, SIGNAL(queuedCall(const char *)),
            this, SLOT(onQueuedCall(const char *)),
            Qt::QueuedConnection);
}

//! Slot which is executed when the event loop of this object
//! runs. Calls all the methods in the queue.
void QueuedInvoker::onQueuedCall(const char *method)
{
    QMutexLocker locker(&callQueueLock);
    callQueue.remove(method);
    locker.unlock();
    if (!QMetaObject::invokeMethod(this, method, Qt::DirectConnection)) {
        qFatal("    *****************\n"
               "Erroneous usage of queueOnce(%s)\n"
               "    *****************\n", method);
    }
}

//! Sets the method \a method to be invoked when the event loop of
//! this object runs next time. If the method was already in the
//! queue, it won't be inserted again.
void QueuedInvoker::queueOnce(const char *method)
{
    QMutexLocker locker(&callQueueLock);
    if (!callQueue.contains(method)) {
        emit queuedCall(method);
        callQueue.insert(method);
    }
}

} // namespace ContextProvider
