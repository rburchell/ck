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
    NanoTree buildMulti();

private slots:
    void keyValue1();
    void keyValue2();
    void keys();
    void keyNode();
    void replaceKey();
    void addKeyValue();
    void keyName();
    void keyValues();
    void dumpTree();
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

    QVariantList type;
    type << QVariant("type");

    QVariantList description;
    description << QVariant("description");
    description << QVariant("some description");
    type << QVariant(description);

    QVariantList tree;
    tree << QVariant(params);
    tree << QVariant(doc);
    tree << QVariant(type);

    return NanoTree(tree);
}

NanoTree NanoTreeUnitTest::buildMulti()
{
    QVariantList param1;
    param1 << QVariant("param");
    param1 << QVariant("value1");

    QVariantList param2;
    param2 << QVariant("param");
    param2 << QVariant("value2");

    QVariantList tree;
    tree << QVariant(param1);
    tree << QVariant(param2);

    return NanoTree(tree);
}

void NanoTreeUnitTest::keyValues()
{
    NanoTree tree = buildMulti();
    QVariantList lst = tree.keyValues("param");
    QCOMPARE(lst.size(), 2);
    QCOMPARE(tree.keyValues("another").size(), 0);
}

void NanoTreeUnitTest::dumpTree()
{
    NanoTree tree = buildBasic();
    QString dump = tree.dumpTree(0);

    QCOMPARE(dump, QString("[\n  [\n    param1\n    value1\n  ]\n  [\n    param2\n    value2\n  ]\n]\n"));
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

    QCOMPARE(lst1.size(), 3);
    QVERIFY(lst1.contains("params"));
    QVERIFY(lst1.contains("doc"));

    QStringList lst2 = tree.keyNode("params").keys();
    QCOMPARE(lst2.size(), 2);
    QVERIFY(lst2.contains("param1"));
    QVERIFY(lst2.contains("param2"));

    QStringList lst3 = tree.keyNode("doc").keys();
    QCOMPARE(lst3.size(), 0);
}

void NanoTreeUnitTest::keyNode()
{
    NanoTree tree1 = buildBasic();
    NanoTree node1 = tree1.keyNode("param1");
    QVERIFY(node1.type() == QVariant::List);
    QCOMPARE(node1.toList().size(), 2);
    QCOMPARE(node1.toList().at(0).type(), QVariant::String);
    QCOMPARE(node1.toList().at(0).toString(), QString("param1"));
    QCOMPARE(node1.toList().at(1).type(), QVariant::String);
    QCOMPARE(node1.toList().at(1).toString(), QString("value1"));
}

void NanoTreeUnitTest::replaceKey()
{
    NanoTree tree1 = buildBasic();

    QVariantList param;
    param << QVariant("param3");
    param << QVariant("value3");

    NanoTree tree2 = tree1.replaceKey("param2", NanoTree(param));
    QCOMPARE(tree2.keyValue("param1").toString(), QString("value1"));
    QCOMPARE(tree2.keyValue("param3").toString(), QString("value3"));
    QCOMPARE(tree2.keyValue("param2").toString(), QString());
}

void NanoTreeUnitTest::addKeyValue()
{
    NanoTree tree1 = buildComplex();
    NanoTree tree2 = tree1.addKeyValue("abra", QVariant("cadabra"));
    QCOMPARE(tree2.keyValue("abra").toString(), QString("cadabra"));
}

void NanoTreeUnitTest::keyName()
{
    QCOMPARE(buildBasic().keyNode("param1").name(), QString("param1"));
}

#include "nanotreeunittest.moc"
QTEST_MAIN(NanoTreeUnitTest);