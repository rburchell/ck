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
IProviderPlugin* pluginFactory(const QString& /*constructionString*/)
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
    sconnect(this, SIGNAL(emitReady()), this, SLOT(onEmitReady()), Qt::QueuedConnection);
    emit emitReady();
}

void TimePlugin::subscribe(QSet<QString> keys)
{
    contextDebug() << keys;
    foreach(const QString& key, keys) {
        emit subscribeFinished(key);
    }
    timer.start();
}

void TimePlugin::unsubscribe(QSet<QString> keys)
{
    timer.stop();
}

void TimePlugin::onEmitReady()
{
    contextDebug();
    emit ready();
}

void TimePlugin::onTimeout()
{
    contextDebug() << "Timeout";
    emit valueChanged("Test.Time", QDateTime::currentDateTime().toString().prepend(prefix));
}

} // end namespace

