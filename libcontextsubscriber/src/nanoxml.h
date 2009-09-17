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

#ifndef NANOXML_H
#define NANOXML_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <QXmlDefaultHandler>
#include <QStack>

class NanoXml : public QObject, QXmlDefaultHandler
{
    Q_OBJECT

public:
    NanoXml(const QString& path);
    const QVariant& root();
    const QString namespaceUri();
    bool didFail();

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

private:
    QList<QVariant> *current; ///< The current list that we're adding to. Top of the stack, kinda.
    QStack<QList <QVariant>* > stack; ///< The stack of lists.
    QVariant rootVariant; ///< The nanodom root. A list of QVariants usually.
    bool failed; ///< This is set by the parser to signify an error.
    QString nspace; ///< Stores the namespace uri.

    void pushList();
    void addValue(const QString& v);
    void popList();

public:
    bool startElement(const QString&, const QString&, const QString &name, const QXmlAttributes &attrs);
    bool endElement(const QString&, const QString&, const QString &name);
    bool startPrefixMapping(const QString &prefix, const QString &uri);
    bool characters(const QString &chars);
};

#endif // NANOXML_H
