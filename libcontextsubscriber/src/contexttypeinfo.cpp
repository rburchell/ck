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
#include "contexttyperegistryinfo.h"

/* Public */

/* The AssocTree in a ContextTypeInfo corresponds directly to what has
   been written in the property declarations.  For example, a property
   declaration of this form

     <key name="Foo.Bar"
          type="string"/>

   will yield a ContextTypeInfo with this tree:

     "string"

   while this declaration

     <key name="Foo.Bar">
       <type>
         <string-enum>
           <foo/>
           <bar/>
         </string-enum>
       </type>
     </key>

   yields this tree

     [ "string-enum", [ "foo" ], [ "bar" ] ]

   The first element in this list is the name of the type, the rest of
   the list are the parameters.

   The first form of just "string" is short-hand for [ "string" ],
   i.e., you can use it when the list of parameters is empty.
*/

/// Returns a ContexTypeInfo where old-style type names (INTEGER,
/// STRING, BOOL) are replaced with new-style type names (integer,
/// string, bool). Returns self if the ContextTypeInfo is already
/// new-style.
ContextTypeInfo ContextTypeInfo::ensureNewTypes()
{
    QString newName;
    QString oldName = name();

    if (oldName == "TRUTH")
        newName = "bool";
    else if (oldName == "INT" || oldName == "INTEGER")
        newName = "integer";
    else if (oldName == "STRING")
        newName = "string";
    else if (oldName == "DOUBLE")
        newName = "double";
    else
        return *this;

    return ContextTypeInfo(QVariant(newName));
}

/// Returns the AssocTree with the type definition for this type.
AssocTree ContextTypeInfo::definition() const
{
    return ContextTypeRegistryInfo::instance()->typeDefinitionForName(name());
}

/* ContextStringEnumInfo
 */

QStringList ContextStringEnumInfo::choices() const
{
    QStringList result;
    foreach (const AssocTree &p, parameters()) {
        result.append (p.name());
    }
    return result;
}
