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

#ifndef __VALUESETMAPTYPE
#define __VALUESETMAPTYPE

#include <QtCore/QMetaType>
#include <QtCore/QMap>
#include <QtDBus/QtDBus>
#include <QDBusVariant>

// you shouldn't use a typedef here, because they are not supported by
// the Q_DECLARE_METATYPE system, please stay with the inheritance

// a{sv}
struct ValueSetMap : public QMap<QString, QDBusVariant>
{
};

Q_DECLARE_METATYPE(ValueSetMap);

#endif // __VALUESETMAPTYPE
