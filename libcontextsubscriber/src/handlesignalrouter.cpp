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

/*! \class HandleSignalRouter

  \brief Routes the <tt>valueChanged()</tt> signal to the correct \c
  PropertyHandle object.

  This is an optimization, so we don't have to connect all of the
  value changes from the same provider to all of the
  <tt>PropertyHandle</tt>s of that provider.
*/

#include "handlesignalrouter.h"
#include "propertyhandle.h"

namespace ContextSubscriber {

HandleSignalRouter* HandleSignalRouter::myInstance = 0;

HandleSignalRouter::HandleSignalRouter()
{
}

HandleSignalRouter* HandleSignalRouter::instance()
{
    if (myInstance == 0) {
        myInstance = new HandleSignalRouter();
    }
    return myInstance;
}

void HandleSignalRouter::onValueChanged(QString key, QVariant value, bool processingSubscription)
{
    PropertyHandle* handle = PropertyHandle::instance(key);
    handle->setValue(value, processingSubscription);
}

} // end namespace
