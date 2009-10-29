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

#include <QtTest/QtTest>
#include <QtCore>
#include "nanotree.h"

class NanoTreeUnitTest : public QObject
{
    Q_OBJECT

private:
    NanoTree buildBasic();

private slots:
    void keyValue();
};

NanoTree NanoTreeUnitTest::buildBasic()
{
    QVariantList param1;
    param1 << QVariant("param1");
    param1 << QVariant("value1");

    QVariantList param2;
    param2 << QVariant("param2");
    param2 << QVariant("value2");

    QVariantList tree;
    tree << QVariant(param1);
    tree << QVariant(param2);

    return NanoTree(tree);
}

void NanoTreeUnitTest::keyValue()
{
    NanoTree basic = buildBasic();
    QCOMPARE(basic.keyValue("param1").toString(), QString("value1"));
    QCOMPARE(basic.keyValue("param2").toString(), QString("value2"));
}

#include "nanotreeunittest.moc"
QTEST_MAIN(NanoTreeUnitTest);
