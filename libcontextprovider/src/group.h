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
class Service;

class GroupPrivate;

class Group : public QObject
{
    Q_OBJECT
    
public:
    explicit Group(QObject* parent = 0);
    ~Group();

    void add(const Property &prop);

    bool isSubscribedTo() const;
    QSet<const Property *> getProperties();

    inline Group &operator<<(const Property &prop)
    { add(prop); return *this; }

    inline Group &operator<<(const Property *prop)
    { add(*prop); return *this; }

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
    GroupPrivate *priv;
};

} // end namespace

#endif
