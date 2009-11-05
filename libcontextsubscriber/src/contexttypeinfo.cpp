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

/* The nano tree in a ContextTypeInfo corresponds directly to what has
   been written in the property declarations.  For example, a property
   declaration of this form

     <key name="Foo.Bar"
          type="string"/>

   will yield a ContextTypeInfo with this nano tree:

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

   yields this nano tree

     [ "string-enum", [ "foo" ], [ "bar" ] ]

   The first element in this list is the name of the type, the rest of
   the list are the parameters.

   The first form of just "string" is short-hand for [ "string" ],
   i.e., you can use it when the list of parameters is empty.
*/

ContextTypeInfo::ContextTypeInfo(QString name)
{
    QVariantList lst;
    lst << QVariant(name);

    tree = NanoTree(lst);
}

ContextTypeInfo::ContextTypeInfo(NanoTree t) : tree(t)
{
}

ContextTypeInfo::ContextTypeInfo(QVariant t) : tree(NanoTree(t))
{
}

ContextTypeInfo::ContextTypeInfo()
{
}

ContextTypeInfo::ContextTypeInfo(const ContextTypeInfo& info) : tree(info.tree)
{
}

/// Assigment operator.
ContextTypeInfo ContextTypeInfo::operator=(const ContextTypeInfo& info)
{
    tree = NanoTree(info.tree);
    return *this;
}

QString ContextTypeInfo::name() const
{
    if (tree.type() == QVariant::List)
        return tree.toList().at(0).toString();
    else
        return tree.toString();
}

QVariantList ContextTypeInfo::parameters() const
{
    if (tree.type() == QVariant::List)
        return tree.toList().mid(1);
    else
        return QVariantList();
}

NanoTree ContextTypeInfo::definition() const
{
    return ContextTypeRegistryInfo::instance()->typeDefinitionForName(name());
}

QString ContextTypeInfo::parameterDoc(QString p) const
{
    return definition().keyValue("params", p, "doc").toString();
}

QVariant ContextTypeInfo::parameterValue(QString p) const
{
    return tree.keyValue(p);
}

QString ContextTypeInfo::doc() const
{
    return definition().keyValue("doc").toString();
}

ContextTypeInfo ContextTypeInfo::base() const
{
    return ContextTypeInfo(definition().keyValue("base").toString());
}
