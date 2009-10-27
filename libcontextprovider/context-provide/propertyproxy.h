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

#ifndef PROPERTYPROXY_H
#define PROPERTYPROXY_H

#include "contextproperty.h"
#include "property.h"

class PropertyProxy : public QObject
{
    Q_OBJECT;
public:
    PropertyProxy(QString key, bool enabled = true, QObject *parent = 0);
    void enable(bool enable);
    QVariant realValue() const;
    QString type() const;
private slots:
    void onValueChanged();
private:
    ContextProvider::Property *provider;
    ContextProperty *subscriber;
    bool enabled;
    QVariant value;
};

#endif
