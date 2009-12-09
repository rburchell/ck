/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef CONTEXTJSON_H
#define CONTEXTJSON_H

#include <QString>
#include <QVariant>
#include <qjson/serializer.h>
#include <qjson/parser.h>

// stolen from glib
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define CKIT_GNUC_UNUSED __attribute__((__unused__))
#else
#define CKIT_GNUC_UNUSED
#endif

namespace ContextSubscriber {

// Please note that it is intentional that this header file contains
// implementation.  This way libcontextsubscriber doesn't have to link
// against libqjson (this would be one more extra dependency), but
// only relatively rare users of this feature have to.

/// Dumps a QVariant into a QString in JSON format.  This is very
/// useful if the value is quite complex (list of maps of lists) and
/// you would like to see it for debugging reasons.
///
/// If you use this function, you have to add libqjson to the list of
/// libraries you link against.
CKIT_GNUC_UNUSED
static QString qVariantToJSON(const QVariant &v) {
    QJson::Serializer serializer;
    return serializer.serialize(v);
}

/// Loads json data into a QVariant.  On parse error it returns QVariant().
CKIT_GNUC_UNUSED
static QVariant jsonToQVariant(const QString &s) {
    QJson::Parser parser;
    bool ok;
    QVariant v = parser.parse(s.toUtf8(), &ok);
    if (ok)
        return QVariant(v);
    else
        return QVariant();
}

} // namespace

#endif
