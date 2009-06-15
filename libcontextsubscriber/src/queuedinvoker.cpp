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

namespace ContextSubscriber {

// FIXME: needs documentation

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

} // namespace
