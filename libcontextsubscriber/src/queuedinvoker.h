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

#ifndef QUEUEDINVOKER_H
#define QUEUEDINVOKER_H

#include <QObject>
#include <QMutex>
#include <QSet>
#include <QString>

namespace ContextSubscriber {

class QueuedInvoker : public QObject
{
    Q_OBJECT

public:
    QueuedInvoker();

private slots:
    void onQueuedCall(const char *method);

signals:
    void queuedCall(const char *method);

protected:
    void queueOnce(const char *method);

private:
    QMutex callQueueLock; ///< Protects the callQueue
    QSet<QString> callQueue; ///< Methods to be invoked
};

} // namespace
#endif
