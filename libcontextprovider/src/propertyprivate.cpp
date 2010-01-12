/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
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

#include "propertyprivate.h"
#include "servicebackend.h"
#include "logging.h"
#include "sconnect.h"
#include "loggingfeatures.h"
#include <time.h>

namespace ContextProvider {

/*!
    \class PropertyPrivate ContextProvider ContextProvider

    \brief The private implementation of Property.

    For each (ServiceBackend*, key) pair there exists only one
    PropertyPrivate; multiple Property objects may share it.
*/


QHash<QPair<ServiceBackend*,QString>, PropertyPrivate*> PropertyPrivate::propertyPrivateMap;

/// Constructor. Register the PropertyPrivate to its ServiceBackend;
/// this will make the Property object appear on D-Bus.
PropertyPrivate::PropertyPrivate(ServiceBackend* serviceBackend, const QString &key, QObject *parent)
    : QObject(parent), refCount(0), serviceBackend(serviceBackend),
      key(key), value(QVariant()),  timestamp(currentTimestamp()), subscribed(false),
      emittedValue(value), emittedTimestamp(timestamp), overheard(false)
{
    // Associate the property to the service backend
    serviceBackend->addProperty(key, this);
}

/// Set value for the PropertyPrivate. Results in a valueChanged
/// signal emission, if 1) the value was different than the current
/// value of the PropertyPrivate, or 2) The provider has overheard
/// another provider setting a different value having a more recent
/// time stamp than our last emission.
void PropertyPrivate::setValue(const QVariant& v)
{
    contextDebug() << F_PROPERTY << "Setting key:" << key << "to type:" << v.typeName();

    // Always store the intention of the provider
    timestamp = currentTimestamp();
    value = v;

    // The provider is setting a different value than it has previously.
    if (value != emittedValue ||
        value.isNull() != emittedValue.isNull() ||
        value.type() != emittedValue.type()) {
        emitValue();
        return;
    }

    // The provider is setting the same value again. But it has
    // overheard a different value after the emission of
    // emittedValue. Thus, emit again.
    if (overheard) {
        emitValue();
        return;
    }
}

/// Emit the valueChanged signal and update the emittedValue and
/// emittedTimestamp. (If subscribed is false, no value is emitted.)
void PropertyPrivate::emitValue()
{
    // No difference between intention and emitted value, nothing
    // happens
    if (emittedTimestamp == timestamp &&
        emittedValue == value &&
        emittedValue.isNull() == value.isNull() &&
        emittedValue.type() == value.type())
        return;

    emittedValue = value;
    emittedTimestamp = timestamp;
    overheard = false;

    // If nobody is subscribed to us, no D-Bus signal needs to be
    // emitted. Note: The bookkeeping above must be done, though.
    if (!subscribed) {
        return;
    }

    QVariantList values;
    if (value.isNull() == false) {
        values << value;
    }
    Q_EMIT valueChanged(values, timestamp);
}

/// Set the PropertyPrivate to subscribed state. If it was in the
/// unsubscribed state, the firstSubscriberAppeared signal is
/// emitted. (Property transmits the signal forward.)
void PropertyPrivate::setSubscribed()
{
    if (subscribed == false) {
        subscribed = true;
        Q_EMIT firstSubscriberAppeared(key);
    }
}

/// Set the PropertyPrivate to unsubscribed state. If it was in the
/// subscribed state, the lastSubscriberDisappeared signal is
/// emitted. (Property transmits the signal forward.)
void PropertyPrivate::setUnsubscribed()
{
    if (subscribed == true) {
      subscribed = false;
      Q_EMIT lastSubscriberDisappeared(key);
    }
}

/// Called by PropertyAdaptor when it has overheard another provider
/// sending a value on D-Bus. Check if the value is different and more
/// recent than the value we've emitted last. If so, emit our value
/// again. This way we ensure that the client gets the correct time
/// stamp for our value.
void PropertyPrivate::updateOverheardValue(const QVariantList& v, const quint64& t)
{
    contextDebug() << "Updating overheard value" << v << t;

    QVariant overheardValue;
    // Update the "overheard" value
    if (v.size() == 0) {
        overheardValue = QVariant();
    }
    else {
        overheardValue = v[0];
    }
    if (t > emittedTimestamp && overheardValue != emittedValue){
        // record that a different and more recent value than the one
        // emitted has been overheard
        overheard = true;
        // emit the value because provider might have a more recent
        // timestamp than t
        emitValue();
    }
}

#define NSECS_IN_SEC 1000000000ULL

/// Compute a unique time stamp for our value. The time stamp is based
/// on monotonic clock and its value is: seconds * 10e9 + nanoseconds.
quint64 PropertyPrivate::currentTimestamp()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    quint64 toReturn = time.tv_nsec;
    toReturn += (NSECS_IN_SEC * time.tv_sec);
    return toReturn;
}

} // end namespace

