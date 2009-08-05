/*
 * Copyright (C) 2008 Nokia Corporation.
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

#ifndef LISTENERS_H
#define LISTENERS_H

#include "context.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include "contextc.h"
#include "contextgroup.h"

namespace ContextProvider {

class PropertyListener;
class Listener;
class ContextGroupListener;

class Listener : public QObject
{
    Q_OBJECT

public:
    Listener(bool clears, ContextProviderSubscriptionChangedCallback cb, void *dt);

private slots:
    void onFirstSubscriberAppeared();
    void onLastSubscriberDisappeared();

protected:
    virtual void clear() = 0;

private:
    ContextProviderSubscriptionChangedCallback callback;
    void *user_data;
    bool clearsOnSubscribe;
};

class PropertyListener : public Listener
{
    Q_OBJECT

public:
    PropertyListener(const QString &k, bool clears, ContextProviderSubscriptionChangedCallback cb, void *dt);

protected:
    virtual void clear();

private:
    Property key;
};

class GroupListener : public Listener 
{
    Q_OBJECT

public:
    GroupListener(const QStringList &keys, bool clears, ContextProviderSubscriptionChangedCallback cb, void *dt);

protected:
    virtual void clear();

private:
    Group group;
    QStringList keyList;
};

} // end namespace

#endif
