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

#ifndef NANOTREE_H
#define NANOTREE_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <QXmlDefaultHandler>
#include <QStack>

class NanoTree : public QVariant
{
public:
    NanoTree() : QVariant() {};
    NanoTree(const QVariant &root) : QVariant(root) {};

    QVariant keySub(const QString &key) const;
    QVariant keyValue(const QString &key) const;
    QVariant keyValue(const QString &key1, const QString &key2) const;
    QVariant keyValue(const QString &key1, const QString &key2, const QString &key3) const;
    QVariant keyValue(const QString &key1, const QString &key2, const QString &key3,
                      const QString &key4) const;
    QVariant keyValue(const QString &key1, const QString &key2, const QString &key3,
                      const QString &key4, const QString &key5) const;
    QVariantList keyValues(const QString &key) const;
    QString stringValue() const;
    NanoTree addStringValue(QString v);
    QStringList keys() const;
    NanoTree replaceKey(QString key, QVariant newNode);
    QVariant keyNode(QString k);

    static QVariant keySub(const QString &key, const QVariant &dom);
    static QVariant keyValue(const QString &key, const QVariant &dom);
    static QVariant keyValue(const QString &key1, const QString &key2, const QVariant &dom);
    static QVariant keyValue(const QString &key1, const QString &key2, const QString &key3,
                             const QVariant &dom);
    static QVariant keyValue(const QString &key1, const QString &key2, const QString &key3,
                                   const QString &key4, const QVariant &dom);
    static QVariant keyValue(const QString &key1, const QString &key2, const QString &key3,
                             const QString &key4, const QString &key5, const QVariant &dom);
    static QVariantList keyValues(const QString &key, const QVariant &dom);
    static QVariant variantValue(const QVariant &dom);
    static QVariant addVariantValue(const QVariant &value, const QVariant &dom);
    static QStringList keys(const QVariant &dom);
    static QVariant replaceKey(QString key, QVariant newNode, const QVariant &dom);
    static QVariant keyNode(QString k, QVariant &dom);

    static QString dumpTree(const QVariant &tree, int level);
};

#endif // NANOTREE_H
