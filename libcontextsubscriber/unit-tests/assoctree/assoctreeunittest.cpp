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
#include "assoctree.h"

class AssocTreeUnitTest : public QObject
{
    Q_OBJECT

private:
    AssocTree buildBasic();
    AssocTree buildComplex();
    AssocTree buildMulti();

private Q_SLOTS:
    void dump();
    void value1();
    void value2();
    void node();
    void name();
    void nodes();
};

AssocTree AssocTreeUnitTest::buildBasic()
{
    QVariantList param1;
    param1 << QVariant("param1");
    param1 << QVariant("value1");

    QVariantList param2;
    param2 << QVariant("param2");
    param2 << QVariant("value2");

    QVariantList tree;
    tree << QVariant("params");
    tree << QVariant(param1);
    tree << QVariant(param2);

    return AssocTree(tree);
}

AssocTree AssocTreeUnitTest::buildComplex()
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
    tree << QVariant("property");
    tree << QVariant(params);
    tree << QVariant(doc);
    tree << QVariant(type);

    return AssocTree(tree);
}

AssocTree AssocTreeUnitTest::buildMulti()
{
    QVariantList param1;
    param1 << QVariant("param");
    param1 << QVariant("value1");

    QVariantList param2;
    param2 << QVariant("param");
    param2 << QVariant("value2");

    QVariantList tree;
    tree << QVariant("params");
    tree << QVariant(param1);
    tree << QVariant(param2);

    return AssocTree(tree);
}

void AssocTreeUnitTest::dump()
{
    AssocTree tree = buildBasic();
    QString dump = tree.dump();

    QCOMPARE(dump, QString("[\n  params\n  [\n    param1\n    value1\n  ]\n  [\n    param2\n    value2\n  ]\n]\n"));
}

void AssocTreeUnitTest::value1()
{
    AssocTree tree = buildBasic();
    QCOMPARE(tree.value("param1").toString(), QString("value1"));
    QCOMPARE(tree.value("param2").toString(), QString("value2"));
}

void AssocTreeUnitTest::value2()
{
    AssocTree tree = buildComplex();
    QCOMPARE(tree.value("params", "param1").toString(), QString("value1"));
    QCOMPARE(tree.value("params", "param2").toString(), QString("value2"));
    QCOMPARE(tree.value("doc").toString(), QString("documentation"));
}

void AssocTreeUnitTest::node()
{
    AssocTree tree1 = buildBasic();
    AssocTree node1 = tree1.node("param1");
    QVERIFY(node1.type() == QVariant::List);
    QCOMPARE(node1.toList().size(), 2);
    QCOMPARE(node1.toList().at(0).type(), QVariant::String);
    QCOMPARE(node1.toList().at(0).toString(), QString("param1"));
    QCOMPARE(node1.toList().at(1).type(), QVariant::String);
    QCOMPARE(node1.toList().at(1).toString(), QString("value1"));
}

void AssocTreeUnitTest::name()
{
    QCOMPARE(buildBasic().node("param1").name(), QString("param1"));
}

void AssocTreeUnitTest::nodes()
{
    AssocTree tree1 = buildBasic();
    
    int i = 0;
    Q_FOREACH (QVariant sub, tree1.nodes())
    {
        QVERIFY(i < 2);
        switch (i) {
        case 0:
            QCOMPARE(sub.toList().at(0).toString(), QString("param1"));
            QCOMPARE(sub.toList().at(1).toString(), QString("value1"));
            break;
        case 1:
            QCOMPARE(sub.toList().at(0).toString(), QString("param2"));
            QCOMPARE(sub.toList().at(1).toString(), QString("value2"));
            break;
        }

        i++;
    }
}

#include "assoctreeunittest.moc"
QTEST_MAIN(AssocTreeUnitTest);
