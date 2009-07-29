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

#ifndef PROVIDER_H
#define PROVIDER_H

#include <QVariant>
#include <QStringList>
#include <QObject>

/*!
    \class Provider

    \brief Base class for all the providers.

    The Provider subclass is expected to provide a list of keys it provides and
    respond to a Provider::initialize() call.
*/

class Provider : public QObject
{
    Q_OBJECT

public:
    /// Returns the list of keys that this provider will provide.
    /// The provider should return here only the keys that it supports and
    /// handles later on.
    virtual QStringList keys() = 0;

    /// Called once to initialize the provider. The provider should here create
    /// the Context property instances and do all the setup work it needs to do.
    virtual void initialize() = 0;
};

#endif // PROVIDER_H
