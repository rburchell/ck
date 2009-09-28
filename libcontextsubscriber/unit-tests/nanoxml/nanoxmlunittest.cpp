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
    void firstLevelAccessor();
    void secondLevelAccessor();
    void thirdLevelAccessor();
    void treeDump();
};

void NanoXmlUnitTest::test1()
{
    NanoXml nano("test1.xml");
    QCOMPARE(nano.didFail(), false);
    QVERIFY(nano.root().type() == QVariant::List);
    QCOMPARE(nano.root().toList().count(), 4);
    QVERIFY(nano.root().toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(0).toString(), QString("key"));

    QVERIFY(nano.root().toList().at(1).type() == QVariant::List);
    QCOMPARE(nano.root().toList().at(1).toList().count(), 2);
    QVERIFY(nano.root().toList().at(1).toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(1).toList().at(0).toString(), QString("name"));
    QVERIFY(nano.root().toList().at(1).toList().at(1).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(1).toList().at(1).toString(), QString("Example.Random"));

    QVERIFY(nano.root().toList().at(2).type() == QVariant::List);
    QCOMPARE(nano.root().toList().at(2).toList().count(), 2);
    QVERIFY(nano.root().toList().at(2).toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(2).toList().at(0).toString(), QString("type"));
    QVERIFY(nano.root().toList().at(2).toList().at(1).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toString(), QString("string"));

    QVERIFY(nano.root().toList().at(3).type() == QVariant::List);
    QCOMPARE(nano.root().toList().at(3).toList().count(), 2);
    QVERIFY(nano.root().toList().at(3).toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(3).toList().at(0).toString(), QString("doc"));
    QVERIFY(nano.root().toList().at(3).toList().at(1).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(3).toList().at(1).toString(), QString("A random property."));

    /* Accessor testing */
    QCOMPARE(nano.keyValue("name"), QVariant("Example.Random"));
    QCOMPARE(nano.keyValue("type"), QVariant("string"));
    QCOMPARE(nano.keyValue("doc"), QVariant("A random property."));
    QCOMPARE(nano.keyValue("wrong"), QVariant());
}

void NanoXmlUnitTest::test2()
{
    NanoXml nano("test2.xml");
    QCOMPARE(nano.didFail(), false);
    QVERIFY(nano.root().type() == QVariant::List);
    QCOMPARE(nano.root().toList().count(), 3);
    QVERIFY(nano.root().toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(0).toString(), QString("key"));

    QVERIFY(nano.root().toList().at(1).type() == QVariant::List);
    QCOMPARE(nano.root().toList().at(1).toList().count(), 2);
    QVERIFY(nano.root().toList().at(1).toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(1).toList().at(0).toString(), QString("name"));
    QVERIFY(nano.root().toList().at(1).toList().at(1).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(1).toList().at(1).toString(), QString("Example.Random"));

    QVERIFY(nano.root().toList().at(2).type() == QVariant::List);
    QCOMPARE(nano.root().toList().at(2).toList().count(), 2);
    QVERIFY(nano.root().toList().at(2).toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(2).toList().at(0).toString(), QString("type"));
    QVERIFY(nano.root().toList().at(2).toList().at(1).type() == QVariant::List);
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toList().count(), 2);
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toList().at(0).type(), QVariant::String);
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toList().at(0).toString(), QString("uniform-list"));
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toList().at(1).type(), QVariant::List);
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toList().at(1).toList().count(), 2);
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toList().at(1).toList().at(0).type(), QVariant::String);
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toList().at(1).toList().at(0).toString(), QString("type"));
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toList().at(1).toList().at(1).type(), QVariant::String);
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toList().at(1).toList().at(1).toString(), QString("number"));

    /* Accessor testing */
    QCOMPARE(nano.keyValue("name"), QVariant("Example.Random"));
    QCOMPARE(nano.keyValue(QString("type"), QString("uniform-list"), QString("type")), QVariant("number"));
}

void NanoXmlUnitTest::test4Comments()
{
    NanoXml nano("test4.xml");
    QCOMPARE(nano.didFail(), false);
    
    QCOMPARE(nano.didFail(), false);
    QVERIFY(nano.root().type() == QVariant::List);
    QCOMPARE(nano.root().toList().count(), 4);
    QVERIFY(nano.root().toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(0).toString(), QString("key"));
    
    QVERIFY(nano.root().toList().at(1).type() == QVariant::List);
    QCOMPARE(nano.root().toList().at(1).toList().count(), 2);
    QVERIFY(nano.root().toList().at(1).toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(1).toList().at(0).toString(), QString("name"));
    QVERIFY(nano.root().toList().at(1).toList().at(1).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(1).toList().at(1).toString(), QString("Example.Key"));
    
    QVERIFY(nano.root().toList().at(2).type() == QVariant::List);
    QCOMPARE(nano.root().toList().at(2).toList().count(), 2);
    QVERIFY(nano.root().toList().at(2).toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(2).toList().at(0).toString(), QString("type"));
    QVERIFY(nano.root().toList().at(2).toList().at(1).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(2).toList().at(1).toString(), QString("string"));

    QVERIFY(nano.root().toList().at(3).type() == QVariant::List);
    QCOMPARE(nano.root().toList().at(3).toList().count(), 2);
    QVERIFY(nano.root().toList().at(3).toList().at(0).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(3).toList().at(0).toString(), QString("doc"));
    QVERIFY(nano.root().toList().at(3).toList().at(1).type() == QVariant::String);
    QCOMPARE(nano.root().toList().at(3).toList().at(1).toString(), QString("A random property."));
}

void NanoXmlUnitTest::test3()
{
    NanoXml nano("test3.xml");
    QCOMPARE(nano.didFail(), false);
    QCOMPARE(nano.namespaceUri(), QString("http://www.test.org/document"));
}

void NanoXmlUnitTest::firstLevelAccessor()
{
    QVariantList list1;
    QVariantList list2;

    list2.append(QVariant("key"));
    list2.append(QVariant("value"));
    list1.append(QVariant(list2));
    QVariant root(list1);

    QCOMPARE(NanoXml::keyValue("key", root), QVariant("value"));
}

void NanoXmlUnitTest::secondLevelAccessor()
{
    QVariantList list1;
    QVariantList list2;
    QVariantList list3;

    list3.append(QVariant("key"));
    list3.append(QVariant("value"));
    list2.append(QVariant("object"));
    list2.append(QVariant(list3));
    list1.append(QVariant(list2));
    QVariant root(list1);



    QCOMPARE(NanoXml::keyValue("object", "key", root), QVariant("value"));
}

void NanoXmlUnitTest::thirdLevelAccessor()
{
    QVariantList list1;
    QVariantList list2;
    QVariantList list3;
    QVariantList list4;

    list4.append(QVariant("key"));
    list4.append(QVariant("value"));
    list3.append(QVariant("object"));
    list3.append(QVariant(list4));
    list2.append(QVariant("collection"));
    list2.append(QVariant(list3));
    list1.append(QVariant(list2));
    QVariant root(list1);

    QVERIFY(NanoXml::keyValue("collection", "object", root) != QVariant());
    QCOMPARE(NanoXml::keyValue("collection", "object", "key", root), QVariant("value"));
}

void NanoXmlUnitTest::broken()
{
    NanoXml nano("broken.xml");
    QCOMPARE(nano.didFail(), true);
    QCOMPARE(nano.root(), QVariant());
}

void NanoXmlUnitTest::doesNotExist()
{
    NanoXml nano("does-not-exist.xml");
    QCOMPARE(nano.didFail(), true);
    QCOMPARE(nano.root(), QVariant());
}

void NanoXmlUnitTest::treeDump()
{
    QVariantList list1;
    QVariantList list2;

    list2.append(QVariant("key"));
    list2.append(QVariant("value"));
    list1.append(QVariant(list2));
    QVariant root(list1);

    QCOMPARE(NanoXml::dumpTree(root, 0), QString("[\n  [\n    key\n    value\n  ]\n]\n"));
}

#include "nanoxmlunittest.moc"
QTEST_MAIN(NanoXmlUnitTest);
