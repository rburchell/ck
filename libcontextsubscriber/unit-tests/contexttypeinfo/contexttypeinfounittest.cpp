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
#include "fileutils.h"
#include "contexttypeinfo.h"
#include "contexttyperegistryinfo.h"

class ContextTypeInfoUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void name();
    void doc();
    void base();
    void empty();
    void definition();
    void parameterDoc();
    void ensureNewTypes();
    void parameterValue();
    void parameterNode();
    void parameters();
    void typeCheck();
};

void ContextTypeInfoUnitTest::initTestCase()
{
    utilSetEnv("CONTEXT_CORE_TYPES",
               utilPathForLocalFile("types.xml"));
}

void ContextTypeInfoUnitTest::name()
{
    QCOMPARE(ContextTypeInfo(QString("bool")).name(), QString("bool"));
    QCOMPARE(ContextTypeInfo(QString("string")).name(), QString("string"));

    QVariantList lst;
    lst << QVariant("int32");
    QCOMPARE(ContextTypeInfo(QVariant(lst)).name(), QString("int32"));
}

void ContextTypeInfoUnitTest::doc()
{
    QCOMPARE(ContextTypeInfo(QString("bool")).doc(), QString("A boolean."));
}

void ContextTypeInfoUnitTest::empty()
{
    ContextTypeInfo cti;
    QCOMPARE(cti.name(), QString());
    QCOMPARE(cti.doc(), QString());
    QVERIFY(cti.parameters().size() == 0);
}

void ContextTypeInfoUnitTest::definition()
{
    AssocTree def = ContextTypeInfo(QString("string")).definition();
    QCOMPARE(def.value("name").toString(), QString("string"));
    QCOMPARE(def.value("doc").toString(), QString("A string."));
}

void ContextTypeInfoUnitTest::base()
{
    ContextTypeInfo base = ContextTypeInfo(QString("integer")).base();
    QCOMPARE(base.name(), QString("number"));
}

void ContextTypeInfoUnitTest::parameterDoc()
{
    ContextTypeInfo typeInfo = ContextTypeInfo(QString("number"));
    QCOMPARE(typeInfo.parameterDoc("min"), QString("Lower bound"));
    QCOMPARE(typeInfo.parameterDoc("max"), QString("Upper bound"));
}

void ContextTypeInfoUnitTest::ensureNewTypes()
{
    QCOMPARE(ContextTypeInfo(QString("INTEGER")).ensureNewTypes().name(),
             QString("integer"));
    QCOMPARE(ContextTypeInfo(QString("INT")).ensureNewTypes().name(),
             QString("integer"));
    QCOMPARE(ContextTypeInfo(QString("TRUTH")).ensureNewTypes().name(),
             QString("bool"));
    QCOMPARE(ContextTypeInfo(QString("STRING")).ensureNewTypes().name(),
             QString("string"));
    QCOMPARE(ContextTypeInfo(QString("DOUBLE")).ensureNewTypes().name(),
             QString("double"));
    QCOMPARE(ContextTypeInfo(QString("bool")).ensureNewTypes().name(),
             QString("bool"));
}

void ContextTypeInfoUnitTest::parameterValue()
{
    QVariantList lst;
    QVariantList minParam;
    lst << QVariant("complex");
    minParam << QVariant("min");
    minParam << QVariant("0");
    lst << QVariant(minParam);
    QVariant tree(lst);

    ContextTypeInfo typeInfo(tree);
    QCOMPARE(typeInfo.parameterValue("min").toString(), QString("0"));
}

void ContextTypeInfoUnitTest::parameterNode()
{
    QVariantList lst;
    QVariantList minParam;
    lst << QVariant("complex");
    minParam << QVariant("min");
    minParam << QVariant("0");
    lst << QVariant(minParam);
    QVariant tree(lst);

    ContextTypeInfo typeInfo(tree);
    QCOMPARE(typeInfo.parameterNode("min").toList().at(1).toString(), QString("0"));
}

void ContextTypeInfoUnitTest::parameters()
{
    QVariantList lst;
    QVariantList minParam;
    lst << QVariant("complex");
    minParam << QVariant("min");
    minParam << QVariant("0");
    lst << QVariant(minParam);
    QVariant tree(lst);

    ContextTypeInfo typeInfo(tree);
    QVariantList params = typeInfo.parameters();
    QCOMPARE(params.size(), 1);
    QCOMPARE(params.at(0).toList().at(0).toString(), QString("min"));

    QVariant variant("double");
    QVERIFY(ContextTypeInfo(variant).parameters().size() == 0);
}

#define LIST(args) QVariant(QVariantList() << args)
#define TI ContextTypeInfo

void ContextTypeInfoUnitTest::typeCheck()
{
    {
        QVariant v(23);
        TI t("number");
        QVERIFY(t.typeCheck(v));
    }

    {
        QVariant v(151.211);
        TI t("number");
        QVERIFY(t.typeCheck(v));
    }

    {
        QVariant v;
        TI t(LIST("number" <<
                  LIST("min" << "100") <<
                  LIST("max" << "200")));

        v = 55;
        QVERIFY(!t.typeCheck(v));
        v = 134.11;
        QVERIFY(t.typeCheck(v));
        v = 250;
        QVERIFY(!t.typeCheck(v));
    }

    {
        QVariant v;
        TI t(LIST("list" <<
                  LIST("min" << "2") <<
                  LIST("type" <<
                       LIST("integer" <<
                            LIST("min" << "2")))));

        v = QVariantList() << 2 << 2 << 3;
        QVERIFY(t.typeCheck(v));
        v = QVariantList() << 1 << 2 << 3;
        QVERIFY(!t.typeCheck(v));
        v = QVariantList() << 4 << 2.2 << 3;
        QVERIFY(!t.typeCheck(v));
        v = QVariantList() << 4;
        QVERIFY(!t.typeCheck(v));
    }

    {
        TI t(LIST("map" <<
                  LIST("foo") <<
                  LIST("bar")));

        QVariantMap v = QVariantMap();
        v.insert("foo", 23);
        v.insert("bar", 11);
        QVERIFY(t.typeCheck(v));
        v.insert("jaj", "nana");
        QVERIFY(!t.typeCheck(v));
    }

    {
        TI t(LIST("map" <<
                  LIST("foo") <<
                  LIST("bar" <<
                       LIST("type" << "string")) <<
                  LIST("allow-other-keys")));

        QVariantMap v = QVariantMap();
        v.insert("foo", 23);
        QVERIFY(t.typeCheck(v));
        v.insert("bar", 11);
        QVERIFY(!t.typeCheck(v));
        v.insert("bar", "is a string");
        QVERIFY(t.typeCheck(v));
        v.insert("jaj", "nana");
        QVERIFY(t.typeCheck(v));
    }
}

#undef TI
#undef LIST

#include "contexttypeinfounittest.moc"
QTEST_MAIN(ContextTypeInfoUnitTest);
