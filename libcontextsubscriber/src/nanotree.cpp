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
QString NanoTree::dumpTree(const QVariant &tree, int level)
{
    QString s;
    for (int i = 0; i < level; i++)
        s += "  ";

    if (tree.type() == QVariant::String)
        s += tree.toString() + "\n";
    else if (tree.type() == QVariant::List) {
        s += "[\n";

        foreach(QVariant v, tree.toList())
            s += NanoTree::dumpTree(v, level + 1);

        for (int i = 0; i < level; i++)
            s += "  ";

        s += "]\n";
    }

    return s;
}

/* Local accessors */

/// Returns the sub (the trailing) after a given \a key in the root nanodom tree.
QVariant NanoTree::keySub(const QString &key) const
{
	return NanoTree::keySub(key, this);
}

/// 1st level accessor. Returns a value for a \a key in the root nanodom tree.
QVariant NanoTree::keyValue(const QString &key) const
{
	return NanoTree::keyValue(key, this);
}

/// Returns the list of QVariants matching the \a key in the root tree. A QVariant
/// node matches if it's a list by and it's first element is \a key.
QVariantList NanoTree::keyValues(const QString &key) const
{
    return NanoTree::keyValues(key, this);
}

/// 2nd level accessor. Returns a value for a \a key1 \a key2 in the root nanodom tree.
QVariant NanoTree::keyValue(const QString &key1, const QString &key2) const
{
	return NanoTree::keyValue(key1, key2, this);
}

/// 3rd level accessor. Returns a value for a \a key1 \a key2 \a key3 in the root nanodom tree.
QVariant NanoTree::keyValue(const QString &key1, const QString &key2, const QString &key3) const
{
	return NanoTree::keyValue(key1, key2, key3, this);
}

/// 4rd level accessor. Returns a value for a \a key1 \a key2 \a key3 \a key4 in the root nanodom tree.
QVariant NanoTree::keyValue(const QString &key1, const QString &key2, const QString &key3,
                           const QString &key4) const
{
	return NanoTree::keyValue(key1, key2, key3, key4, this);
}

/// 5th level accessor. Returns a value for a \a key1 \a key2 \a key3 \a key4 \a key5 in the root nanodom tree.
QVariant NanoTree::keyValue(const QString &key1, const QString &key2, const QString &key3,
                           const QString &key4, const QString &key5) const
{
	return NanoTree::keyValue(key1, key2, key3, key4, key5, this);
}

QStringList NanoTree::keys() const
{
    return NanoTree::keys(this);
}

/// Returns the value of the current tree root as string. The value is a collected
/// non-list node that represents the value of the current key.
QString NanoTree::stringValue() const
{
    return NanoTree::variantValue(this).toString();
}

/// Add a string value to the current NanoTree. A value is a lose (non-list) QVariant
/// attached to the tree root. (\a dom).
NanoTree NanoTree::addStringValue(QString v)
{
    return NanoTree(NanoTree::addVariantValue(QVariant(v), *this));
}

/// Replaces the key \a key with a new NanoTree node \a newNode.
NanoTree NanoTree::replaceKey(QString key, QVariant newNode)
{
    return NanoTree(NanoTree::replaceKey(key, newNode, *this));
}

/// Similiar to NanoTree::keyValue (1st level accessor) but returns not the
/// value of the key \a k but the whole node of it. Useful to move nodes around
/// when building/changing the NanoTrees.
QVariant NanoTree::keyNode(QString k)
{
    return NanoTree::keyNode(k, *this);
}

/* Static accessors */

/// Returns the sub (the trailing) after a given \a key in the specified \a dom tree.
QVariant NanoTree::keySub(const QString &key, const QVariant &dom)
{
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect the dom to be a list...
    if (dom.type() != QVariant::List)
        return QVariant();

    foreach(QVariant child, dom.toList())
    {
        if (child.type() == QVariant::List && child.toList().count() > 0 &&
            child.toList().at(0) == keyVariant) {
            QVariantList lst = child.toList();
            lst.removeAt(0);
            return QVariant(lst);
        }
    }

    return QVariant();
}

/// Returns the list of QVariants matching the \a key in a given tree. A QVariant
/// node matches if it's a list by and it's first element is \a key.
QVariantList NanoTree::keyValues(const QString &key, const QVariant &dom)
{
    QVariantList lst;
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect the dom to be a list...
    if (dom.type() != QVariant::List)
        return lst;

    foreach(QVariant child, dom.toList())
    {
        if (child.type() == QVariant::List && child.toList().count() > 0 &&
            child.toList().at(0) == keyVariant) {
            lst.append(child);
        }
    }

    return lst;
}

/// 1st level accessor. Returns a value for a \a key in the given \a dom tree.
QVariant NanoTree::keyValue(const QString &key, const QVariant &dom)
{
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect the dom to be a list...
    if (dom.type() != QVariant::List)
        return QVariant();

    foreach(QVariant child, dom.toList())
    {
        if (child.type() == QVariant::List && child.toList().count() >= 2 &&
            child.toList().at(0) == keyVariant)
            return child.toList().at(1);
    }

    return QVariant();
}

/// 2nd level accessor. Returns a value for a \a key1 \a key2 in the given \a dom tree.
QVariant NanoTree::keyValue(const QString &key1, const QString &key2, const QVariant &dom)
{
    return NanoTree::keyValue(key2, keySub(key1, dom));
}

/// 3rd level accessor. Returns a value for a \a key1 \a key2 \a key3 in the given \a dom tree.
QVariant NanoTree::keyValue(const QString &key1, const QString &key2, const QString &key3,
                                 const QVariant &dom)
{
    return NanoTree::keyValue(key2, key3, keySub(key1, dom));
}

/// 4rd level accessor. Returns a value for a \a key1 \a key2 \a key3 \a key4 in the given \a dom tree.
QVariant NanoTree::keyValue(const QString &key1, const QString &key2, const QString &key3,
                           const QString &key4, const QVariant &dom)
{
    return NanoTree::keyValue(key2, key3, key4, keySub(key1, dom));
}

/// 5th level accessor. Returns a value for a \a key1 \a key2 \a key3 \a key4 \a key5 in the given \a dom tree.
QVariant NanoTree::keyValue(const QString &key1, const QString &key2, const QString &key3,
                           const QString &key4, const QString &key5, const QVariant &dom)
{
    return NanoTree::keyValue(key2, key3, key4, key5, keySub(key1, dom));
}

/// Returns the value of the the dom tree as QVariant. The value is a collected
/// non-list node that represents the value of the current key.
QVariant NanoTree::variantValue(const QVariant &dom)
{
    if (dom.type() != QVariant::List)
        return QVariant();

    if (dom.toList().size() < 2)
        return QVariant();

    foreach(QVariant v, dom.toList()) {
       if (v.type() != QVariant::List)
           return v;
    }

    return QVariant();
}

/// Add a QVariant value to the given tree. A value is a lose (non-list) QVariant
/// attached to the current list (\a dom).
QVariant NanoTree::addVariantValue(const QVariant &value, const QVariant &dom)
{
    if (dom.type() != QVariant::List)
        return dom;

    QVariantList lst = dom.toList();
    lst << value;
    return QVariant(lst);
}

QStringList NanoTree::keys(const QVariant &dom)
{
    if (dom.type() != QVariant::List)
        return QStringList();

    QStringList lst;

    foreach(QVariant child, dom.toList()) {
        if (child.type() == QVariant::List && child.toList().count() >= 2)
            lst << child.toList().at(0).toString();
    }

    return lst;
}

/// Replaces the key \a key with a new NanoTree node \a newNode.
QVariant NanoTree::replaceKey(QString key, QVariant newNode, const QVariant &dom)
{
    if (dom.type() != QVariant::List)
        return dom;

    const QVariant keyVariant(key); // So we can directly compare...
    QVariantList lst;

    foreach(QVariant child, dom.toList())
    {
        if (child.type() == QVariant::List && child.toList().count() > 0 &&
            child.toList().at(0) == keyVariant) {
            lst << newNode;
        } else
            lst << child;
    }

    return QVariant(lst);
}

/// Similiar to NanoTree::keyValue (1st level accessor) but returns not the
/// value of the key \a k but the whole node of it. Useful to move nodes around
/// when building/changing the NanoTrees.
QVariant NanoTree::keyNode(QString key, QVariant &dom)
{
    const QVariant keyVariant(key); // So we can directly compare...

    // We expect the dom to be a list...
    if (dom.type() != QVariant::List)
        return QVariant();

    foreach(QVariant child, dom.toList())
    {
        if (child.type() == QVariant::List && child.toList().count() >= 2 &&
            child.toList().at(0) == keyVariant)
            return child;
    }

    return QVariant();
}

