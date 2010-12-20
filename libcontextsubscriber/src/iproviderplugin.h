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

#ifndef IPROVIDERPLUGIN_H
#define IPROVIDERPLUGIN_H

#include "timedvalue.h"

#include <QObject>
#include <QVariant>

namespace ContextSubscriber {

/* This is not a public API of ContextKit, please do not write third
 * party plugins for the ContextKit client library without first
 * contacting us.
 */

class IProviderPlugin : public QObject
{
    Q_OBJECT
public:
    virtual void subscribe(QSet<QString> keys) = 0;
    virtual void unsubscribe(QSet<QString> keys) = 0;
    virtual void blockUntilReady() = 0;
    virtual void blockUntilSubscribed(const QString& key) = 0;

Q_SIGNALS:
    void ready();
    void failed(QString error);
    void subscribeFinished(QString key);
    void subscribeFinished(QString key, TimedValue timedvalue);
    void subscribeFailed(QString failedKey, QString error);
    void valueChanged(QString key, QVariant value);
    void valueChanged(QString key, TimedValue timedvalue);
};

typedef IProviderPlugin* (*PluginFactoryFunc)(QString constructionString);

}

#endif
