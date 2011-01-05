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

// mock implementation

#ifndef CONTEXTKITPLUGIN_H
#define CONTEXTKITPLUGIN_H

#include "iproviderplugin.h"
#include "timedvalue.h"

#include <QString>
#include <QDBusConnection>
#include <QSet>
#include <QVariant>

extern "C" {
    ContextSubscriber::IProviderPlugin* contextKitPluginFactory(QString constructionString);
}

namespace ContextSubscriber {
class ContextKitPlugin : public IProviderPlugin
{
    Q_OBJECT

public:
    void subscribe(QSet<QString> keys);
    void unsubscribe(QSet<QString> keys);
    void blockUntilReady();
    void blockUntilSubscribed(const QString& key);

Q_SIGNALS:
    void ready();
    void failed(QString error);
    void subscribeFinished(QString key);
    void subscribeFinished(QString key, TimedValue value);
    void subscribeFailed(QString failedKey, QString error);
    void valueChanged(QString key, QVariant value);
    void valueChanged(QString key, TimedValue value);

private:
    QSet<QString> subscribeRequested;
    QSet<QString> unsubscribeRequested;

    friend class ProviderUnitTests;
};


}

#endif
