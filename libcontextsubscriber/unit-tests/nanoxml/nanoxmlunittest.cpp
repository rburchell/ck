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
#include "nanoxml.h"
#include "fileutils.h"

class NanoXmlUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void broken();
    void doesNotExist();
    void test1();
    void test2();
    void test3();
    void test4Comments();
    void test5();
};

void NanoXmlUnitTest::test1()
{
    NanoXml parser(LOCAL_FILE("test1.xml"));
    QCOMPARE(parser.didFail(), false);
    AssocTree tree = parser.result();

    QVERIFY(tree.type() == QVariant::List);
    QCOMPARE(tree.toList().count(), 4);
    QVERIFY(tree.toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(0).toString(), QString("key"));

    QVERIFY(tree.toList().at(1).type() == QVariant::List);
    QCOMPARE(tree.toList().at(1).toList().count(), 2);
    QVERIFY(tree.toList().at(1).toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(1).toList().at(0).toString(), QString("name"));
    QVERIFY(tree.toList().at(1).toList().at(1).type() == QVariant::String);
    QCOMPARE(tree.toList().at(1).toList().at(1).toString(), QString("Example.Random"));

    QVERIFY(tree.toList().at(2).type() == QVariant::List);
    QCOMPARE(tree.toList().at(2).toList().count(), 2);
    QVERIFY(tree.toList().at(2).toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(2).toList().at(0).toString(), QString("type"));
    QVERIFY(tree.toList().at(2).toList().at(1).type() == QVariant::String);
    QCOMPARE(tree.toList().at(2).toList().at(1).toString(), QString("string"));

    QVERIFY(tree.toList().at(3).type() == QVariant::List);
    QCOMPARE(tree.toList().at(3).toList().count(), 2);
    QVERIFY(tree.toList().at(3).toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(3).toList().at(0).toString(), QString("doc"));
    QVERIFY(tree.toList().at(3).toList().at(1).type() == QVariant::String);
    QCOMPARE(tree.toList().at(3).toList().at(1).toString(), QString("A random property."));
}

void NanoXmlUnitTest::test2()
{
    NanoXml parser(LOCAL_FILE("test2.xml"));
    QCOMPARE(parser.didFail(), false);
    AssocTree tree = parser.result();

    QVERIFY(tree.type() == QVariant::List);
    QCOMPARE(tree.toList().count(), 3);
    QVERIFY(tree.toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(0).toString(), QString("key"));

    QVERIFY(tree.toList().at(1).type() == QVariant::List);
    QCOMPARE(tree.toList().at(1).toList().count(), 2);
    QVERIFY(tree.toList().at(1).toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(1).toList().at(0).toString(), QString("name"));
    QVERIFY(tree.toList().at(1).toList().at(1).type() == QVariant::String);
    QCOMPARE(tree.toList().at(1).toList().at(1).toString(), QString("Example.Random"));

    QVERIFY(tree.toList().at(2).type() == QVariant::List);
    QCOMPARE(tree.toList().at(2).toList().count(), 2);
    QVERIFY(tree.toList().at(2).toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(2).toList().at(0).toString(), QString("type"));
    QVERIFY(tree.toList().at(2).toList().at(1).type() == QVariant::List);
    QCOMPARE(tree.toList().at(2).toList().at(1).toList().count(), 2);
    QCOMPARE(tree.toList().at(2).toList().at(1).toList().at(0).type(), QVariant::String);
    QCOMPARE(tree.toList().at(2).toList().at(1).toList().at(0).toString(), QString("uniform-list"));
    QCOMPARE(tree.toList().at(2).toList().at(1).toList().at(1).type(), QVariant::List);
    QCOMPARE(tree.toList().at(2).toList().at(1).toList().at(1).toList().count(), 2);
    QCOMPARE(tree.toList().at(2).toList().at(1).toList().at(1).toList().at(0).type(), QVariant::String);
    QCOMPARE(tree.toList().at(2).toList().at(1).toList().at(1).toList().at(0).toString(), QString("type"));
    QCOMPARE(tree.toList().at(2).toList().at(1).toList().at(1).toList().at(1).type(), QVariant::String);
    QCOMPARE(tree.toList().at(2).toList().at(1).toList().at(1).toList().at(1).toString(), QString("number"));
}

void NanoXmlUnitTest::test3()
{
    NanoXml parser(LOCAL_FILE("test3.xml"));
    QCOMPARE(parser.didFail(), false);
    QCOMPARE(parser.namespaceUri(), QString("http://www.test.org/document"));
}

void NanoXmlUnitTest::test4Comments()
{
    NanoXml parser(LOCAL_FILE("test4.xml"));
    QCOMPARE(parser.didFail(), false);
    AssocTree tree = parser.result();

    QVERIFY(tree.type() == QVariant::List);
    QCOMPARE(tree.toList().count(), 4);
    QVERIFY(tree.toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(0).toString(), QString("key"));

    QVERIFY(tree.toList().at(1).type() == QVariant::List);
    QCOMPARE(tree.toList().at(1).toList().count(), 2);
    QVERIFY(tree.toList().at(1).toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(1).toList().at(0).toString(), QString("name"));
    QVERIFY(tree.toList().at(1).toList().at(1).type() == QVariant::String);
    QCOMPARE(tree.toList().at(1).toList().at(1).toString(), QString("Example.Key"));

    QVERIFY(tree.toList().at(2).type() == QVariant::List);
    QCOMPARE(tree.toList().at(2).toList().count(), 2);
    QVERIFY(tree.toList().at(2).toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(2).toList().at(0).toString(), QString("type"));
    QVERIFY(tree.toList().at(2).toList().at(1).type() == QVariant::String);
    QCOMPARE(tree.toList().at(2).toList().at(1).toString(), QString("string"));

    QVERIFY(tree.toList().at(3).type() == QVariant::List);
    QCOMPARE(tree.toList().at(3).toList().count(), 2);
    QVERIFY(tree.toList().at(3).toList().at(0).type() == QVariant::String);
    QCOMPARE(tree.toList().at(3).toList().at(0).toString(), QString("doc"));
    QVERIFY(tree.toList().at(3).toList().at(1).type() == QVariant::String);
    QCOMPARE(tree.toList().at(3).toList().at(1).toString(), QString("A random property."));
}

void NanoXmlUnitTest::test5()
{
    NanoXml parser(LOCAL_FILE("test5.xml"));
    QCOMPARE(parser.didFail(), false);
    AssocTree tree = parser.result();
    QCOMPARE(tree.value("param1").toString(), QString("value1"));
}

void NanoXmlUnitTest::broken()
{
    NanoXml parser(LOCAL_FILE("broken.xml"));
    QCOMPARE(parser.didFail(), true);
}

void NanoXmlUnitTest::doesNotExist()
{
    NanoXml parser("does-not-exist.xml");
    QCOMPARE(parser.didFail(), true);
}

#include "nanoxmlunittest.moc"
QTEST_MAIN(NanoXmlUnitTest);
