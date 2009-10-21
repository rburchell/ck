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
      key(key), value(QVariant()),  timestamp(currentTimestamp())
{
}

void PropertyPrivate::setValue(const QVariant& v)
{
    // Even if the value was the same as before, we still update the
    // time stamp and send the valueChanged signal.

    contextDebug() << F_PROPERTY << "Setting key:" << key << "to type:" << v.typeName();
    value = v;
    timestamp = currentTimestamp();

    QVariantList values;
    if (value.isNull() == false) {
        values << value;
    }
    contextDebug() << "signal should be emitted";
    emit valueChanged(values, timestamp);
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

