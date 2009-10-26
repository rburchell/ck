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
#include <math.h>

namespace ContextProvider {

QHash<QPair<ServiceBackend*,QString>, PropertyPrivate*> PropertyPrivate::propertyPrivateMap;

PropertyPrivate::PropertyPrivate(ServiceBackend* serviceBackend, const QString &key, QObject *parent)
    : QObject(parent), refCount(0), serviceBackend(serviceBackend),
      key(key), value(QVariant()),  timestamp(currentTimestamp()), subscribed(false),
      emittedValue(value), emittedTimestamp(timestamp), overheard(false)
{
    // Associate the property to the service backend
    serviceBackend->addProperty(key, this);
}

PropertyPrivate::~PropertyPrivate()
{
    // Remove the property from the service backend
    serviceBackend->removeProperty(key);
}

/// Increase the reference count by one. Property calls this.
void PropertyPrivate::ref()
{
    refCount++;
}

/// Decrease the reference count by one. Property calls this. If the
/// reference count goes to zero, schedule the PropertyPrivate
/// instance to be deleted.
void PropertyPrivate::unref()
{
    refCount--;

    if (refCount == 0) {
        // For now, remove the PropertyPrivate from the instance
        // map. If this is changed, we need to check carefully what
        // happens if a new ServiceBackend is created with the same
        // memory address as a previous ServiceBackend (which was
        // destructed), and having the old PropertyPrivate store the
        // ServiceBackend pointer to that memory address.
        QPair<ServiceBackend*, QString> key = propertyPrivateMap.key(this);
        if (key.second != "")
            propertyPrivateMap.remove(key);
        else
            contextCritical() << "PropertyPrivate couldn't find itself in the instance store";
        deleteLater(); // "delete this" would be probably unsafe
    }
}

void PropertyPrivate::setValue(const QVariant& v)
{
    contextDebug() << F_PROPERTY << "Setting key:" << key << "to type:" << v.typeName();

    // Always store the intention of the provider
    timestamp = currentTimestamp();
    value = v;

    // The provider is setting a different value than it has previously.
    if (value != emittedValue || value.isNull() != emittedValue.isNull()) {
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

void PropertyPrivate::emitValue()
{
    // No difference between intention and emitted value, nothing happens
    if (emittedTimestamp == timestamp && emittedValue == value
        && emittedValue.isNull() == value.isNull())
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
    emit valueChanged(values, timestamp);
}

void PropertyPrivate::setSubscribed()
{
    subscribed = true;
    emit firstSubscriberAppeared(key);
}

void PropertyPrivate::setUnsubscribed()
{
    subscribed = false;
    emit lastSubscriberDisappeared(key);
}

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
    if (t > emittedTimestamp &&  overheardValue != emittedValue){
        // record that a different and more recent value than the one
        // emitted has been overheard
        overheard = true;
        // emit the value because provider might have a more recent
        // timestamp than t
        emitValue();
    }
}

quint64 PropertyPrivate::currentTimestamp()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    quint64 toReturn = time.tv_nsec;
    toReturn += ((quint64)pow(10,9) * time.tv_sec);
    return toReturn;
}


} // end namespace

