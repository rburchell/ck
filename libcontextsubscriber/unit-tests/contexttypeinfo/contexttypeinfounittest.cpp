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
#include "contexttypeinfo.h"
#include "contexttyperegistryinfo.h"

ContextTypeRegistryInfo* ContextTypeRegistryInfo::registryInstance = new ContextTypeRegistryInfo();

/* Mocked ContextTypeRegistryInfo */

ContextTypeRegistryInfo::ContextTypeRegistryInfo()
{
}

ContextTypeRegistryInfo* ContextTypeRegistryInfo::instance()
{
    return registryInstance;
}

NanoTree ContextTypeRegistryInfo::typeDefinitionForName(QString name)
{
    if (name == "double") {
        QVariantList tree;
        QVariantList doc;
        QVariantList name;
        name << QVariant("name");
        name << QVariant("double");
        doc << QVariant("doc");
        doc << QVariant("double doc");
        tree << QVariant("type");
        tree << QVariant(name);
        tree << QVariant(doc);
        return ContextTypeInfo(QVariant(tree));
    } else if (name == "complex") {
        QVariantList tree;
        QVariantList doc;
        QVariantList base;
        QVariantList name;
        QVariantList params;
        QVariantList p1;
        QVariantList p1Doc;

        name << QVariant("name");
        name << QVariant("complex");

        doc << QVariant("doc");
        doc << QVariant("complex doc");

        base << QVariant("base");
        base << QVariant("double");

        p1 << QVariant("p1");
        p1Doc << QVariant("doc");
        p1Doc << QVariant("p1 doc");
        p1 << QVariant(p1Doc);
        params << QVariant("params");
        params << QVariant(p1);

        tree << QVariant("type");
        tree << QVariant(name);
        tree << QVariant(params);
        tree << QVariant(doc);
        tree << QVariant(base);
        return ContextTypeInfo(QVariant(tree));
    } else
        return ContextTypeInfo();
}


class ContextTypeInfoUnitTest : public QObject
{
    Q_OBJECT

private slots:
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
};

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
    QCOMPARE(ContextTypeInfo(QString("double")).doc(), QString("double doc"));
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
    NanoTree def = ContextTypeInfo(QString("double")).definition();
    QCOMPARE(def.keyValue("name").toString(), QString("double"));
    QCOMPARE(def.keyValue("doc").toString(), QString("double doc"));
}

void ContextTypeInfoUnitTest::base()
{
    ContextTypeInfo base = ContextTypeInfo(QString("complex")).base();
    QCOMPARE(base.name(), QString("double"));
    QCOMPARE(base.doc(), QString("double doc"));
}

void ContextTypeInfoUnitTest::parameterDoc()
{
    ContextTypeInfo typeInfo = ContextTypeInfo(QString("complex"));
    QCOMPARE(typeInfo.parameterDoc("p1"), QString("p1 doc"));

}

void ContextTypeInfoUnitTest::ensureNewTypes()
{
    QCOMPARE(ContextTypeInfo(QString("INTEGER")).ensureNewTypes().name(), QString("int32"));
    QCOMPARE(ContextTypeInfo(QString("INT")).ensureNewTypes().name(), QString("int32"));
    QCOMPARE(ContextTypeInfo(QString("TRUTH")).ensureNewTypes().name(), QString("bool"));
    QCOMPARE(ContextTypeInfo(QString("STRING")).ensureNewTypes().name(), QString("string"));
    QCOMPARE(ContextTypeInfo(QString("DOUBLE")).ensureNewTypes().name(), QString("double"));
    QCOMPARE(ContextTypeInfo(QString("bool")).ensureNewTypes().name(), QString("bool"));
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

#include "contexttypeinfounittest.moc"
QTEST_MAIN(ContextTypeInfoUnitTest);
