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

/*
This is a test plugin for customer tests.
*/

#ifndef TIMEPLUGIN_H
#define TIMEPLUGIN_H

#include "iproviderplugin.h" // For IProviderPlugin definition
#include <QTimer>

using ContextSubscriber::IProviderPlugin;

extern "C" {
    IProviderPlugin* pluginFactory(QString constructionString);
}

namespace ContextSubscriberTime
{

class TimePlugin : public IProviderPlugin
{
    Q_OBJECT

public:
    explicit TimePlugin();
    virtual void subscribe(QSet<QString> keys);
    virtual void unsubscribe(QSet<QString> keys);
    virtual void waitUntilReadyAndBlock();
    virtual void waitForSubscriptionAndBlock(const QString& key);

private Q_SLOTS:
    void onTimeout();

private:
    QTimer timer;
    QString prefix;
};
}

#endif
