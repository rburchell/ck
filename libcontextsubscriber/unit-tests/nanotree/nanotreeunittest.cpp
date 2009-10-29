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
    NanoTree buildComplex();

private slots:
    void keyValue1();
    void keyValue2();
    void keys();
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

NanoTree NanoTreeUnitTest::buildComplex()
{
    QVariantList params;
    params << QVariant("params");

    QVariantList param1;
    param1 << QVariant("param1");
    param1 << QVariant("value1");

    QVariantList param2;
    param2 << QVariant("param2");
    param2 << QVariant("value2");

    params << QVariant(param1);
    params << QVariant(param2);

    QVariantList doc;
    doc << QVariant("doc");
    doc << QVariant("documentation");

    QVariantList tree;
    tree << QVariant(params);
    tree << QVariant(doc);

    return NanoTree(tree);
}

void NanoTreeUnitTest::keyValue1()
{
    NanoTree tree = buildBasic();
    QCOMPARE(tree.keyValue("param1").toString(), QString("value1"));
    QCOMPARE(tree.keyValue("param2").toString(), QString("value2"));
}

void NanoTreeUnitTest::keyValue2()
{
    NanoTree tree = buildComplex();
    QCOMPARE(tree.keyValue("params", "param1").toString(), QString("value1"));
    QCOMPARE(tree.keyValue("params", "param2").toString(), QString("value2"));
    QCOMPARE(tree.keyValue("doc").toString(), QString("documentation"));
}

void NanoTreeUnitTest::keys()
{
    NanoTree tree = buildComplex();
    QStringList lst1 = tree.keys();

    QCOMPARE(lst1.size(), 2);
    QVERIFY(lst1.contains("params"));
    QVERIFY(lst1.contains("doc"));

    QStringList lst2 = tree.keyNode("params").keys();
    QCOMPARE(lst2.size(), 2);
    QVERIFY(lst2.contains("param1"));
    QVERIFY(lst2.contains("param2"));

    QStringList lst3 = tree.keyNode("doc").keys();
    QCOMPARE(lst3.size(), 0);
}

#include "nanotreeunittest.moc"
QTEST_MAIN(NanoTreeUnitTest);
