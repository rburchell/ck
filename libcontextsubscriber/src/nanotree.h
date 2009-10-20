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

class NanoTree : public QObject
{
    Q_OBJECT

public:
    NanoTree(const QVariant &root);
    NanoTree(const NanoTree &tree);
    const QVariant& root() const;

    QVariant keySub(const QString &key);
    QVariant keyValue(const QString &key);
    QVariant keyValue(const QString &key1, const QString &key2);
    QVariant keyValue(const QString &key1, const QString &key2, const QString &key3);
    QVariant keyValue(const QString &key1, const QString &key2, const QString &key3,
                      const QString &key4);
    QVariant keyValue(const QString &key1, const QString &key2, const QString &key3,
                      const QString &key4, const QString &key5);
    QVariantList keyValues(const QString &key);

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

    static QString dumpTree(const QVariant &tree, int level);

protected:
    QVariant rootVariant; ///< The nanodom root. A list of QVariants usually.
};

#endif // NANOTREE_H
