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

#ifndef INFOKEYDATA_H
#define INFOKEYDATA_H

#include <QString>

/*!
    \struct InfoKeyData

    \brief Simple storage class that groups info about a given key.

    This struct is not a part of the public API. It's used by the InfoXmlBackend that
    keeps in memory a hash of InfoKeyData instances for each key.
*/

struct InfoKeyData
{
    QString name; ///< Name of the we're storing data for.
    QString type; ///< Type of the key.
    QString doc; ///< Doc for the key.
};

#endif // INFOKEYDATA_H
