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
        tree << QVariant("int32");
        doc << QVariant("doc");
        doc << QVariant("double doc");
        tree << QVariant(doc);
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
    void parseDoubleType();
    void parseCustomDoubleType();
    void parseUniformList();
    void parseMap();
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

void ContextTypeInfoUnitTest::parseDoubleType()
{
    /*
    NanoXml parser(LOCAL_FILE("double.xml"));
    QCOMPARE(parser.didFail(), false);

    ContextTypeInfo typeInfo(parser.result());
    QCOMPARE(typeInfo.name(), QString("double"));
    QCOMPARE(typeInfo.parameterDoc("min"), QString("Minimum value"));
    QCOMPARE(typeInfo.parameterDoc("max"), QString("Maximum value"));
    QCOMPARE(typeInfo.doc(), QString("A double value within the given limits."));
    */

    //QStringList params = typeInfo.parameters();
    //QCOMPARE(params.size(), 2);
    //QVERIFY(params.contains(QString("min")));
    //QVERIFY(params.contains(QString("max")));
}

void ContextTypeInfoUnitTest::parseCustomDoubleType()
{
    /*
    NanoXml parser(LOCAL_FILE("custom-double.xml"));
    QCOMPARE(parser.didFail(), false);

    ContextTypeInfo typeInfo(parser.result());
    QCOMPARE(typeInfo.name(), QString("custom-double"));
    QCOMPARE(typeInfo.parameterValue("min"), QString("1"));
    QCOMPARE(typeInfo.parameterValue("max"), QString("666"));
    QCOMPARE(typeInfo.parameterDoc("min"), QString("Minimum value"));
    QCOMPARE(typeInfo.parameterDoc("max"), QString("Maximum value"));
    QCOMPARE(typeInfo.doc(), QString("A double value that represents the level of hell in you."));
    */
}

void ContextTypeInfoUnitTest::parseUniformList()
{
    /*
    NanoXml parser(LOCAL_FILE("uniform-list.xml"));
    QCOMPARE(parser.didFail(), false);

    ContextUniformListTypeInfo listInfo(parser.result());
    QCOMPARE(listInfo.base().name(), QString("list"));
    QCOMPARE(listInfo.name(), QString("uniform-list"));
    ContextTypeInfo elementTypeInfo = listInfo.elementType();
    QCOMPARE(elementTypeInfo.name(), QString("double"));
    */
}

void ContextTypeInfoUnitTest::parseMap()
{
    /*
    NanoXml parser(LOCAL_FILE("person.xml"));
    QCOMPARE(parser.didFail(), false);

    ContextMapTypeInfo personInfo(parser.result());
    QCOMPARE(personInfo.name(), QString("person"));
    QCOMPARE(personInfo.base().name(), QString("map"));
    QCOMPARE(personInfo.keyDoc("name"), QString("Name of the person"));
    QCOMPARE(personInfo.keyDoc("surname"), QString("Surname of the person"));
    QCOMPARE(personInfo.keyDoc("age"), QString("Age of the person"));

    QCOMPARE(personInfo.keyType("name").name(), QString("string"));
    QCOMPARE(personInfo.keyType("surname").name(), QString("string"));
    QCOMPARE(personInfo.keyType("age").name(), QString("int32"));
    */
}

#include "contexttypeinfounittest.moc"
QTEST_MAIN(ContextTypeInfoUnitTest);
