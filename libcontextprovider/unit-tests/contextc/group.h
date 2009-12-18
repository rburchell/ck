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

#ifndef GROUP_H
#define GROUP_H

#include <QObject>
#include <QSet>
#include <QStringList>

namespace ContextProvider {

class Property;

class Group : public QObject
{
    Q_OBJECT
    
public:
    explicit Group(QObject* parent = 0);
    ~Group();

    QStringList keyList;
    void fakeFirst();
    void fakeLast();
    
    QSet<const Property *> getProperties();

    void add(const Property &prop);

    inline Group &operator<<(const Property &prop)
    { add(prop); return *this; }

    inline Group &operator<<(const Property *prop)
    { add(*prop); return *this; }

Q_SIGNALS:
    void firstSubscriberAppeared();
    void lastSubscriberDisappeared();

private:
    QSet<const Property *> props;
};

} // end namespace

#endif
