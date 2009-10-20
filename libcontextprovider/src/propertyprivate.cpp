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

namespace ContextProvider {


PropertyPrivate::PropertyPrivate(ServiceBackend* serviceBackend, const QString &key, QObject *parent)
    : QObject(parent), serviceBackend(serviceBackend), key(key), value(QVariant())
{
}

void PropertyPrivate::setValue(const QVariant& v)
{
    if (v == value && v.isNull() == value.isNull()) {
        // Same value, skip
        // FIXME: should the time stamp be updated?
        return;
    }

    contextDebug() << F_PROPERTY << "Setting key:" << key << "to type:" << v.typeName();
    value = v;

    QVariantList values;
    values << value;
    emit valueChanged(values, 0); // FIXME: timestamp
}


} // end namespace

