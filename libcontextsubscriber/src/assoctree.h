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

#ifndef ASSOCTREE_H
#define ASSOCTREE_H

#include <QVariant>
#include <QObject>

class AssocTree : public QVariant
{
public:
    AssocTree() : QVariant() {};
    AssocTree(const QVariant &root) : QVariant(root) {};

    QString dump(int level = 0) const;
    QString dumpXML(int level = 0) const;

    QString name() const;
    QVariant value() const;
    AssocTree node(const QString &name) const;

    QVariant value(const QString &name1) const;
    QVariant value(const QString &name1,
                   const QString &name2) const;
    QVariant value(const QString &name1,
                   const QString &name2,
                   const QString &name3) const;
    QVariant value(const QString &name1,
                   const QString &name2,
                   const QString &name3,
                   const QString &name4) const;
    QVariant value(const QString &name1,
                   const QString &name2,
                   const QString &name3,
                   const QString &name4,
                   const QString &name5) const;

    AssocTree node(const QString &name1,
                   const QString &name2) const;
    AssocTree node(const QString &name1,
                   const QString &name2,
                   const QString &name3) const;
    AssocTree node(const QString &name1,
                   const QString &name2,
                   const QString &name3,
                   const QString &name4) const;
    AssocTree node(const QString &name1,
                   const QString &name2,
                   const QString &name3,
                   const QString &name4,
                   const QString &name5) const;

    const QVariantList nodes() const;
    AssocTree filterOut(const QString &name) const;
};

#endif // ASSOCTREE_H
