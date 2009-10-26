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

#include "handlesignalrouter.h"
#include "propertyhandle.h"

namespace ContextSubscriber {

/*!
  \class HandleSignalRouter

  \brief Routes the <tt>valueChanged()</tt> and the
  <tt>subscribeFinished()</tt> signals to the correct \c
  PropertyHandle object.

  This is an optimization, so we don't have to connect all of the
  providers to all of the <tt>PropertyHandle</tt>s of that provider.
*/


HandleSignalRouter HandleSignalRouter::myInstance;

HandleSignalRouter::HandleSignalRouter()
{
}

HandleSignalRouter* HandleSignalRouter::instance()
{
    return &myInstance;
}

void HandleSignalRouter::onValueChanged(QString key)
{
    PropertyHandle* handle = PropertyHandle::instance(key);
    handle->onValueChanged();
}

void HandleSignalRouter::onSubscribeFinished(Provider *provider, QString key)
{
    PropertyHandle* handle = PropertyHandle::instance(key);
    handle->setSubscribeFinished(provider);
}

} // end namespace
