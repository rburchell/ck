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

#ifndef SIGNALGROUPER_H
#define SIGNALGROUPER_H

// FIXME: Add this class (and others) to a namespace.

#include <QObject>
#include <QSet>
#include <QStringList>

class Context;

class SignalGrouper : public QObject
{
    Q_OBJECT
    
public:
    explicit SignalGrouper(QSet<Context*> propertiesToWatch, QObject* parent = 0);
    explicit SignalGrouper(QStringList propertiesToWatch, QObject* parent = 0);
    ~SignalGrouper();

    bool isSubscribedTo() const;

signals:
    /// Emitted when the group of Context objects is subscribed
    /// to. I.e., when none of them were subscribed to and now some of
    /// them were subscribed to.
    void firstSubscriberAppeared();
    
    /// Emitted when the group of Context objects is subscribed
    /// from. I.e., when some of them were subscribed to and now all of
    /// them were unsubscribed from.
    void lastSubscriberDisappeared();
    
private slots:
    void onFirstSubscriberAppeared();
    void onLastSubscriberDisappeared();

private:
    int propertiesSubscribedTo; ///< Number of Context objects currently subscribed to.
};


#endif
