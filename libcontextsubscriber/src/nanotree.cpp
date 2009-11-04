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

#include "nanotree.h"
#include <QDebug>

/// Dumps a QVariant into a multi-line string for debugging purposes.
QString NanoTree::dumpTree(int level) const
{
    QString s;
    for (int i = 0; i < level; i++)
        s += "  ";

    if (type() == QVariant::String)
        s += toString() + "\n";
    else if (type() == QVariant::List) {
        s += "[\n";

        foreach(QVariant v, toList())
            s += NanoTree(v).dumpTree(level + 1);

        for (int i = 0; i < level; i++)
            s += "  ";

        s += "]\n";
    }

    return s;
}

/// Assuming this tree represents a key, return the name of the key.
/// (first item).
NanoTree NanoTree::keyName() const
{
    if (type() != QVariant::List)
        return NanoTree();

    if (toList().size() == 0)
        return NanoTree();

    return NanoTree(toList().at(0));
}

/// Returns the list of QVariants matching the \a key in a given tree. A QVariant
/// node matches if it's a list by and it's first element is \a key.
QVariantList NanoTree::keyValues(const QString &key) const
{
    QVariantList lst;
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect the dom to be a list...
    if (type() != QVariant::List)
        return lst;

    foreach(QVariant child, toList())
    {
        if (child.type() == QVariant::List && child.toList().count() > 0 &&
            child.toList().at(0) == keyVariant) {
            lst.append(child);
        }
    }

    return lst;
}

/// Returns the sub (the trailing) after a given \a key in the root nanodom tree.
NanoTree NanoTree::keySub(const QString &key) const
{
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect us to be a list...
    if (type() != QVariant::List)
        return NanoTree();

    foreach(QVariant child, toList())
    {
        if (child.type() == QVariant::List && child.toList().count() > 0 &&
            child.toList().at(0) == keyVariant) {
            QVariantList lst = child.toList();
            lst.removeAt(0);
            return NanoTree(QVariant(lst));
        }
    }

    return NanoTree();
}

/// 1st level accessor. Returns a value for a \a key in the root nanodom tree.
NanoTree NanoTree::keyValue(const QString &key) const
{
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect us to be a list...
    if (type() != QVariant::List)
        return NanoTree();

    foreach(QVariant child, toList())
    {
        if (child.type() == QVariant::List && child.toList().count() >= 2 &&
            child.toList().at(0) == keyVariant)
            return NanoTree(child.toList().at(1));
    }

    return NanoTree();
}

/// 2nd level accessor. Returns a value for a \a key1 \a key2 in the root nanodom tree.
NanoTree NanoTree::keyValue(const QString &key1, const QString &key2) const
{
    return keySub(key1).keyValue(key2);
}

/// 3rd level accessor. Returns a value for a \a key1 \a key2 \a key3 in the root nanodom tree.
NanoTree NanoTree::keyValue(const QString &key1, const QString &key2, const QString &key3) const
{
    return keySub(key1).keyValue(key2, key3);
}

/// 4rd level accessor. Returns a value for a \a key1 \a key2 \a key3 \a key4 in the root nanodom tree.
NanoTree NanoTree::keyValue(const QString &key1, const QString &key2, const QString &key3,
                            const QString &key4) const
{
    return keySub(key1).keyValue(key2, key3, key4);
}

/// 5th level accessor. Returns a value for a \a key1 \a key2 \a key3 \a key4 \a key5 in the root nanodom tree.
NanoTree NanoTree::keyValue(const QString &key1, const QString &key2, const QString &key3,
                            const QString &key4, const QString &key5) const
{
    return keySub(key1).keyValue(key2, key3, key4, key5);
}

QStringList NanoTree::keys() const
{
    if (type() != QVariant::List)
        return QStringList();

    QStringList lst;

    foreach(QVariant child, toList()) {
        if (child.type() == QVariant::List && child.toList().count() >= 2)
            lst << child.toList().at(0).toString();
    }

    return lst;
}

/// Returns the value of the current tree root as string. The value is a collected
/// non-list node that represents the value of the current key.
QString NanoTree::stringValue() const
{
    if (type() != QVariant::List)
        return QString();

    if (toList().size() < 2)
        return QString();

    QVariantList lst = toList();
    lst.removeFirst();
    foreach(QVariant v, lst) {
       if (v.type() != QVariant::List)
           return v.toString();
    }

    return QString();
}

NanoTree NanoTree::addKeyValue(QString key, QVariant v) const
{
    if (type() != QVariant::List)
        return *this;

    QVariantList lst = toList();
    QVariantList keyNodeList;
    keyNodeList << QVariant(key);
    keyNodeList << v;
    lst << QVariant(keyNodeList);
    return NanoTree(QVariant(lst));
}

/// Replaces the key \a key with a new NanoTree node \a newNode.
NanoTree NanoTree::replaceKey(QString key, NanoTree newNode) const
{
    if (type() != QVariant::List)
        return *this;

    const QVariant keyVariant(key); // So we can directly compare...
    QVariantList lst;

    foreach(QVariant child, toList())
    {
        if (child.type() == QVariant::List && child.toList().count() > 0 &&
            child.toList().at(0) == keyVariant) {
            lst << newNode;
        } else
            lst << child;
    }

    return NanoTree(QVariant(lst));
}

/// Similiar to NanoTree::keyValue (1st level accessor) but returns not the
/// value of the key \a k but the whole node of it. Useful to move nodes around
/// when building/changing the NanoTrees.
NanoTree NanoTree::keyNode(QString key) const
{
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect the dom to be a list...
    if (type() != QVariant::List)
        return QVariant();

    foreach(QVariant child, toList())
    {
        if (child.type() == QVariant::List && child.toList().count() >= 2 &&
            child.toList().at(0) == keyVariant)
            return child;
    }

    return NanoTree();
}

