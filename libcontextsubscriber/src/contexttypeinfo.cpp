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

#include "contexttypeinfo.h"
#include "logging.h"

ContextTypeInfo::ContextTypeInfo(const NanoTree &nanoTree)
{
    QVariant root = nanoTree.root();

    if (root.type() != QVariant::List &&
        root.toList().at(0).toString() != "type") {
        contextWarning() << "Nanotree does not represent type!";
        return;
    }

    if (root.toList().at(1).type() == QVariant::String)
        typeName = root.toList().at(1).toString();
    else if (root.toList().at(1).type() == QVariant::List)
        typeName = root.toList().at(1).toList().at(0).toString();
    else
        typeName = "";
}

QString ContextTypeInfo::name() const
{
    return typeName;
}
