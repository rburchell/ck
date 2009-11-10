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

/// Buids a simple (no parameters) type with the given name.
ContextTypeInfo::ContextTypeInfo(QString name)
{
    QVariantList lst;
    lst << QVariant(name);

    tree = NanoTree(lst);
}

/// Butilds a type based on the given tree.
ContextTypeInfo::ContextTypeInfo(NanoTree t) : tree(t)
{
}

/// Butilds a type based on the given variant.
ContextTypeInfo::ContextTypeInfo(QVariant t) : tree(NanoTree(t))
{
}

/// Butilds an empty type.
ContextTypeInfo::ContextTypeInfo()
{
}

/// Butilds a type cloning the other type
ContextTypeInfo::ContextTypeInfo(const ContextTypeInfo& info) : tree(info.tree)
{
}

/// Returns a ContexTypeInfo where old-style type names (INTEGER, STRING, BOOL)
/// are replaced with new-style type names (int32, string, bool). Returns self
/// if the ContextTypeInfo is already new-style.
ContextTypeInfo ContextTypeInfo::ensureNewTypes()
{
    QString newName;
    QString oldName = name();

    if (oldName == "TRUTH")
        newName = "bool";
    else if (oldName == "INT" || oldName == "INTEGER")
        newName = "int32";
    else if (oldName == "STRING")
        newName = "string";
    else if (oldName == "DOUBLE")
        newName = "double";
    else
        return *this;

    QVariantList newTree;
    newTree << QVariant(newName);
    newTree << tree.toList().mid(1);
    return ContextTypeInfo(newTree);
}

/// Assigment operator.
ContextTypeInfo ContextTypeInfo::operator=(const ContextTypeInfo& info)
{
    tree = NanoTree(info.tree);
    return *this;
}

/// Returns the name of the type (such as 'double').
QString ContextTypeInfo::name() const
{
    if (tree.type() == QVariant::List)
        return tree.toList().at(0).toString();
    else
        return tree.toString();
}

/// Returns the list of parameter nodes. Each node is a QVariant with key
/// descriptor and the key value.
QVariantList ContextTypeInfo::parameters() const
{
    if (tree.type() == QVariant::List)
        return tree.toList().mid(1);
    else
        return QVariantList();
}

/// Returns the NanoTree (a QVariant) with the type definition for this type.
NanoTree ContextTypeInfo::definition() const
{
    return ContextTypeRegistryInfo::instance()->typeDefinitionForName(name());
}

/// Returns a documentation for the parameter with name \a p.
QString ContextTypeInfo::parameterDoc(QString p) const
{
    return definition().keyValue("params", p, "doc").toString();
}

/// Returns the current parameter value for the parameter \a p.
QVariant ContextTypeInfo::parameterValue(QString p) const
{
    return tree.keyValue(p);
}

/// Returns the QVariant node (key + value) for the parameter \a p.
NanoTree ContextTypeInfo::parameterNode(QString p) const
{
    return tree.keyNode(p);
}

/// Returns the document for this type. The documentation is fetched according to the
/// type definition.
QString ContextTypeInfo::doc() const
{
    return definition().keyValue("doc").toString();
}

/// Returns the base type for this type.
ContextTypeInfo ContextTypeInfo::base() const
{
    return ContextTypeInfo(definition().keyValue("base").toString());
}

/* ContextStringEnumInfo
 */

QStringList ContextStringEnumInfo::choices() const
{
    QStringList result;
    foreach (const NanoTree &p, parameters()) {
        result.append (p.name());
    }
    return result;
}

QString ContextStringEnumInfo::choiceDoc(const QString &choice) const
{
    return parameterNode(choice).keyValue("doc").toString();
}
