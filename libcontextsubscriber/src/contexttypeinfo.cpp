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
#include "loggingfeatures.h"
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

ContextTypeInfo::ContextTypeInfo(const AssocTree &tree) : AssocTree (tree)
{
}

ContextTypeInfo::ContextTypeInfo(const QVariant &tree) : AssocTree (tree)
{
}

ContextTypeInfo::ContextTypeInfo()
{
}

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
        newName = "number";
    else
        return *this;

    return ContextTypeInfo(QVariant(newName));
}

/// Verifies if \a value is acceptable as a representative of the type that
/// this ContextTypeInfo object describes.
bool ContextTypeInfo::typeCheck(const QVariant &value) const
{
    QString me = name();

    // First check against the parametrized base type.
    ContextTypeInfo baseType(base());
    if (!baseType.isNull()) {
        // Combine type instance parameters with default parameters of the
        // base type.  (Suboptimal.)
        if (baseType.type() != QVariant::List)
            baseType = AssocTree(QVariantList() << baseType.name());
        foreach (AssocTree p, parameters()) {
            baseType = baseType.filterOut(p.name());
            baseType = AssocTree(baseType.toList() << p);
        }
        if (!baseType.typeCheck(value))
            return false;
    }

    // Now let's see our hardwired knowledge about our types.
    QVariant::Type vtype = value.type();
    if (me == "value") {
        return true;
    }
    else if (me == "bool") {
        bool ok = vtype == QVariant::Bool;
        if (!ok)
            contextWarning() << F_TYPES << value << "is not a bool";
        return ok;
    }
    else if (me == "number") {
        if (!value.canConvert(QVariant::Double)) {
            contextWarning() << F_TYPES << value << "is not a number";
            return false;
        }
        QVariant min = parameterValue("min");
        QVariant max = parameterValue("max");
        double v = value.toDouble();
        if (!min.isNull() && v < min.toDouble()) {
            contextWarning() << F_TYPES << v << "below minimum:" << min.toDouble();
            return false;
        }
        if (!max.isNull() && max.toDouble() < v) {
            contextWarning() << F_TYPES << v << "above maximum:" << max.toDouble();
            return false;
        }
        return true;
    }
    else if (me == "integer") {
        bool ok = (vtype == QVariant::Int ||
                   vtype == QVariant::UInt ||
                   vtype == QVariant::LongLong ||
                   vtype == QVariant::ULongLong);
        if (!ok)
            contextWarning() << F_TYPES << value << "is not an integer type";
        return ok;
    }
    else if (me == "string") {
        bool ok = vtype == QVariant::String;
        if (!ok)
            contextWarning() << F_TYPES << value << "is not a string";
        return ok;
    }
    else if (me == "list") {
        if (vtype != QVariant::List) {
            contextWarning() << F_TYPES << value << "is not a list";
            return false;
        }
        QVariant minlen = parameterValue("min");
        QVariant maxlen = parameterValue("max");
        ContextTypeInfo eltype = parameterValue("type");
        QVariantList vl = value.toList();
        if (!minlen.isNull() && vl.size() < minlen.toInt()) {
            contextWarning() << F_TYPES << value << "shorter than minimum";
            return false;
        }
        if (!maxlen.isNull() && vl.size() > maxlen.toInt()) {
            contextWarning() << F_TYPES << value << "length over maximum";
            return false;
        }
        if (!eltype.isNull())
            foreach (QVariant el, vl)
                if (!eltype.typeCheck(el)) {
                    contextWarning() << F_TYPES << "element" << el
                                     << "is of incorrect type";
                    return false;
                }
        return true;
    }
    else if (me == "map") {
        if (vtype != QVariant::Map) {
            contextWarning() << F_TYPES << value << "is not a map";
            return false;
        }
        QHash<QString, ContextTypeInfo> allowedKeys;
        bool othersAllowed = false;
        foreach (ContextTypeInfo keyInfo, parameters()) {
            QString keyname = keyInfo.name();
            if (keyname == "allow-other-keys") {
                othersAllowed = true;
                continue;
            }
            allowedKeys.insert(keyname, keyInfo.parameterValue("type"));
        }
        if (allowedKeys.size() == 0)
            return true;
        QMapIterator<QString, QVariant> it(value.toMap());
        while (it.hasNext()) {
            it.next();
            QString key = it.key();
            if (allowedKeys.contains(key)) {
                if (allowedKeys[key].isNull())
                    continue;
                if (!allowedKeys[key].typeCheck(it.value())) {
                    contextWarning() << F_TYPES << "value for" << key
                                     << "(" << it.value() << ")"
                                     << "is of incorrect type";
                    return false;
                }
            } else if (!othersAllowed) {
                contextWarning() << F_TYPES << "unknown key" << key;
                return false;
            }
        }
        return true;
    }
    else if (me == "string-enum") {
        QSet<QString> enumerators;
        foreach (AssocTree enumerator, parameters())
            enumerators.insert(enumerator.name());
        bool ok = enumerators.contains(value.toString());
        if (!ok)
            contextWarning() << F_TYPES
                             << value << "is not valid in this string-enum";
        return ok;
    }
    else if (me == "int-enum") {
        QSet<int> enumerators;
        foreach (AssocTree enumerator, parameters())
            enumerators.insert(enumerator.value("value").toInt());
        bool ok = enumerators.contains(value.toInt());
        if (!ok)
            contextWarning() << F_TYPES << value << "is not valid in this int-enum";
        return ok;
    }
    return true;
}

bool ContextTypeInfo::hasBase(QString wanted, int depth) const
{
    if (name() == wanted)
        return true;

    if (base() == AssocTree())
        return false;

    if (depth == 0)
        return false;

    return base().hasBase(wanted, depth-1);
}

QVariantList ContextTypeInfo::parameters() const
{
    return nodes();
}

QVariant ContextTypeInfo::parameterValue(QString p) const
{
    return value(p);
}

AssocTree ContextTypeInfo::parameterNode(QString p) const
{
    return node(p);
}

QString ContextTypeInfo::parameterDoc(QString p) const
{
    return definition().value("params", p, "doc").toString();
}

QString ContextTypeInfo::doc() const
{
    return definition().value("doc").toString();
}

ContextTypeInfo ContextTypeInfo::base() const
{
    return ContextTypeInfo(definition().value("base"));
}

/// Returns the AssocTree with the type definition for this type.
AssocTree ContextTypeInfo::definition() const
{
    return ContextTypeRegistryInfo::instance()->typeDefinitionForName(name());
}

/* ContextStringEnumInfo
 */

ContextStringEnumInfo::ContextStringEnumInfo(const ContextTypeInfo &info) : ContextTypeInfo (info)
{
}

QString ContextStringEnumInfo::choiceDoc(const QString &choice) const
{
    return value (choice, "doc").toString();
}

QStringList ContextStringEnumInfo::choices() const
{
    QStringList result;
    foreach (const AssocTree &p, parameters()) {
        result.append (p.name());
    }
    return result;
}

ContextListInfo::ContextListInfo(const ContextTypeInfo &info) : ContextTypeInfo (info)
{
}

ContextTypeInfo ContextListInfo::type()
{
    return ContextTypeInfo(parameterValue("type"));
}
