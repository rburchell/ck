/*
 * Copyright (C) 2009 Nokia Corporation.
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

#include "assoctree.h"
#include <QDebug>

/// Dumps a QVariant into a multi-line string for debugging purposes.
QString AssocTree::dump(int level) const
{
    QString s;
    for (int i = 0; i < level; i++)
        s += "  ";

    if (type() == QVariant::String)
        s += toString() + "\n";
    else if (type() == QVariant::List) {
        s += "[\n";

        foreach(QVariant v, toList())
            s += AssocTree(v).dump(level + 1);

        for (int i = 0; i < level; i++)
            s += "  ";

        s += "]\n";
    }

    return s;
}

/// Serializes the tree in NanoXml format.
QString AssocTree::dumpXML(int level) const
{
    QString s;
    for (int i = 0; i < level; i++)
        s += "  ";

    if (type() == QVariant::String)
        s += toString() + '\n';
    else if (type() == QVariant::List) {
        const QVariantList &children = nodes();
        if (children.size() == 0) {
            s += "<" + name() + "/>\n";
        } else {
            s += "<" + name() + ">\n";
            foreach(QVariant v, children)
                s += AssocTree(v).dumpXML(level + 1);
            for (int i = 0; i < level; i++)
                s += "  ";
            s += "</" + name() + ">\n";
        }
    }
    return s;
}

/// Returns the name of this association tree.
QString AssocTree::name() const
{
    if (type() == QVariant::String)
        return toString();

    if (type() != QVariant::List
        || toList().size() < 1)
        return QString();

    return toList().at(0).toString();
}

/// Returns the sub-tree with the given name
AssocTree AssocTree::node(const QString &name) const
{
    const QVariant nameVariant(name); // So we can directly compare...

    if (type() != QVariant::List)
        return AssocTree();

    foreach(const QVariant &child, toList())
    {
        if (child.type() == QVariant::List
            && child.toList().count() >= 1
            && child.toList().at(0) == nameVariant)
            return AssocTree(child);
    }

    return AssocTree();
}

/// Returns the sub-tree named \a name2 of the sub-tree named \a name1 
AssocTree AssocTree::node(const QString &name1,
                          const QString &name2) const
{
    return node(name1).node(name2);
}

AssocTree AssocTree::node(const QString &name1,
                          const QString &name2,
                          const QString &name3) const
{
    return node(name1).node(name2).node(name3);
}

AssocTree AssocTree::node(const QString &name1,
                          const QString &name2,
                          const QString &name3,
                          const QString &name4) const
{
    return node(name1).node(name2).node(name3),node(name4);
}

AssocTree AssocTree::node(const QString &name1,
                          const QString &name2,
                          const QString &name3,
                          const QString &name4,
                          const QString &name5) const
{
    return node(name1).node(name2).node(name3),node(name4).node(name5);
}

/// Returns the value of this tree.
QVariant AssocTree::value() const
{
    if (type() != QVariant::List)
        return QVariant();

    if (toList().size() < 2)
        return QVariant();

    return toList().at(1);
}

/// Returns the value of the sub-tree named name1.
QVariant AssocTree::value(const QString &name1) const
{
    return node(name1).value();
}

QVariant AssocTree::value(const QString &name1,
                          const QString &name2) const
{
    return node(name1, name2).value();
}

QVariant AssocTree::value(const QString &name1,
                          const QString &name2,
                          const QString &name3) const
{
    return node(name1, name2, name3).value();
}

QVariant AssocTree::value(const QString &name1,
                          const QString &name2,
                          const QString &name3,
                          const QString &name4) const
{
    return node(name1, name2, name3, name4).value();
}

QVariant AssocTree::value(const QString &name1,
                          const QString &name2,
                          const QString &name3,
                          const QString &name4,
                          const QString &name5) const
{
    return node(name1, name2, name3, name4, name5).value();
}

/// Returns a list of all sub-trees.
const QVariantList AssocTree::nodes() const
{
    if (type() == QVariant::List)
        return toList().mid(1);
    else
        return QVariantList();
}
