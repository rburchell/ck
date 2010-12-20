/*
 * Copyright (C) 2009 Nokia Corporation.
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

#include "timeplugin.h"
#include "sconnect.h"

#include "logging.h"

#include <QDateTime>

/// The factory method for constructing the IPropertyProvider instance.
IProviderPlugin* pluginFactory(QString /*constructionString*/)
{
    // Note: it's the caller's responsibility to delete the plugin if
    // needed.
    return new ContextSubscriberTime::TimePlugin();
}

namespace ContextSubscriberTime {

TimePlugin::TimePlugin()
{
    contextDebug();
    prefix = TIME_PLUGIN_PREFIX;
    timer.setInterval(2000);
    sconnect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    QMetaObject::invokeMethod(this, "ready", Qt::QueuedConnection);
}

void TimePlugin::subscribe(QSet<QString> keys)
{
    contextDebug() << keys;
    Q_FOREACH(const QString& key, keys) {
        Q_EMIT subscribeFinished(key);
    }
    timer.start();
}

void TimePlugin::unsubscribe(QSet<QString> keys)
{
    timer.stop();
}

void TimePlugin::waitUntilReadyAndBlock()
{
}

void TimePlugin::waitForSubscriptionAndBlock(const QString& key)
{
}

void TimePlugin::onTimeout()
{
    contextDebug() << "Timeout";
    Q_EMIT valueChanged("Test.Time", QDateTime::currentDateTime().toString().prepend(prefix));
}

} // end namespace
