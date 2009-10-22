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
      key(key), value(QVariant()),  timestamp(currentTimestamp()), overheardTimestamp(timestamp), subscribed(false)
{
}

void PropertyPrivate::setValue(const QVariant& v)
{
    contextDebug() << F_PROPERTY << "Setting key:" << key << "to type:" << v.typeName();

    // Always update the time stamp, whether or not we will emit a
    // D-Bus signal
    timestamp = currentTimestamp();

    if (v != value || v.isNull() != value.isNull()) {
        // Provider sets a different value -> emit always
        value = v;
        emitValue();
        return;
    }

    // Now the value set by the provider is the same that we already
    // have

    if (v != overheardValue || v.isNull() != overheardValue.isNull()) {
        // We have overheard a value which is different from the value
        // we had previously (and which we try to set now)
        if (timestamp > overheardTimestamp) {
            // Our value is more recent
            emitValue();
        }
    }
}

void PropertyPrivate::emitValue()
{
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

void PropertyPrivate::updateOverheardValue(const QVariantList& v, const qlonglong& t)
{
    contextDebug() << "Updating overheard value" << v << t;
    if (t > overheardTimestamp) {
        // Update the "overheard" value
        if (v.size() == 0) {
            overheardValue = QVariant();
        }
        else {
            overheardValue = v[0];
        }

        if (timestamp > t) {
            // We have a more recent value; make sure that it's
            // emitted. Note: here we might do an unnecessary emission,
            // but we don't care
            emitValue();
        }
    }
}


qlonglong PropertyPrivate::currentTimestamp()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    qlonglong toReturn = time.tv_nsec;
    toReturn += ((qlonglong)pow(10,9) * time.tv_sec);
    return toReturn;
}


} // end namespace

