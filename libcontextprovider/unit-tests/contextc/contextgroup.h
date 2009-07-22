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

#ifndef CONTEXTGROUP_H
#define CONTEXTGROUP_H

// FIXME: Add this class (and others) to a namespace.

#include <QObject>
#include <QSet>
#include <QStringList>

class Context;

class ContextGroup : public QObject
{
    Q_OBJECT
    
public:
    explicit ContextGroup(QStringList propertiesToWatch, QObject* parent = 0);
    ~ContextGroup();
    QStringList keyList;
    void fakeFirst();
    void fakeLast();
    
signals:
    void firstSubscriberAppeared();
    void lastSubscriberDisappeared();

private:
};

#endif
