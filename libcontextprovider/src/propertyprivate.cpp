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
    : QObject(parent), serviceBackend(serviceBackend),
      key(key), value(QVariant()),  timestamp(currentTimestamp()), subscribed(false), 
      emittedValue(value), emittedTimestamp(timestamp), overheard(false)
{
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
    if (!subscribed) {
        return;
    }
    // No difference between intention and emitted value, nothing happens
    if (emittedTimestamp == timestamp && emittedValue == value
        && emittedValue.isNull() == value.isNull())
        return;

    emittedValue = value;
    emittedTimestamp = timestamp;
    overheard = false;

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

