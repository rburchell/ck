/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef PROPERTYHANDLE_H
#define PROPERTYHANDLE_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QSet>

class ContextPropertyInfo;
class PropertyProvider;

class PropertyHandle : public QObject
{
    Q_OBJECT

public:
    void subscribe();
    void unsubscribe();

    QString key() const;
    QVariant value() const;
    bool isSubscribePending() const;
    PropertyProvider* provider() const;
    const ContextPropertyInfo* info() const;

    static PropertyHandle* instance(const QString& key);

    void setValue(QVariant newValue, PropertyProvider* provider, bool allowSameValue);

signals:
    void valueChanged();

private slots:
    void onSubscribeFinished(QSet<QString> keys);

private:
    PropertyHandle(const QString& key);

    PropertyProvider *myProvider; ///< Provider of this property
    ContextPropertyInfo *myInfo; ///< Introspection instance
    unsigned int subscribeCount; ///< Number of subscribed ContextProperty objects subscribed to this property
    bool subscribePending; ///< True when the subscription has been started, but hasn't been finished yet
                           ///  (used by the waitForSubscription() feature)
    QString myKey; ///< Name of this context property
    QVariant myValue; ///< Cached value of this context property
    static QMap<QString, PropertyHandle*> handleInstances; ///< Container for singletons, see the \c instance(key) call
};
#endif
