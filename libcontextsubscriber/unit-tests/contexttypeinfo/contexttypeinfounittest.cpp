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

class ContextTypeInfoUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void basicTypes();
    void resolveTypeName();
    void parseDoubleType();
    void parseCustomDoubleType();
    void parseUniformList();
};

void ContextTypeInfoUnitTest::basicTypes()
{
    QCOMPARE(ContextTypeInfo::boolType().name(), QString("bool"));
    QCOMPARE(ContextTypeInfo::stringType().name(), QString("string"));
    QCOMPARE(ContextTypeInfo::int64Type().name(), QString("int64"));
    QCOMPARE(ContextTypeInfo::int32Type().name(), QString("int32"));
    QCOMPARE(ContextTypeInfo::doubleType().name(), QString("double"));
    QCOMPARE(ContextTypeInfo::nullType().name(), QString(""));
}

void ContextTypeInfoUnitTest::resolveTypeName()
{
    QCOMPARE(ContextTypeInfo::resolveTypeName("TRUTH").name(), QString("bool"));
    QCOMPARE(ContextTypeInfo::resolveTypeName("INT").name(), QString("int32"));
    QCOMPARE(ContextTypeInfo::resolveTypeName("INTEGER").name(), QString("int32"));
    QCOMPARE(ContextTypeInfo::resolveTypeName("DOUBLE").name(), QString("double"));
    QCOMPARE(ContextTypeInfo::resolveTypeName("STRING").name(), QString("string"));
    QCOMPARE(ContextTypeInfo::resolveTypeName("int32").name(), QString("int32"));
    QCOMPARE(ContextTypeInfo::resolveTypeName("int64").name(), QString("int64"));
    QCOMPARE(ContextTypeInfo::resolveTypeName("double").name(), QString("double"));
    QCOMPARE(ContextTypeInfo::resolveTypeName("bool").name(), QString("bool"));
    QCOMPARE(ContextTypeInfo::resolveTypeName("string").name(), QString("string"));
}

void ContextTypeInfoUnitTest::parseDoubleType()
{
    NanoXml nano(LOCAL_FILE("double.xml"));
    QCOMPARE(nano.didFail(), false);

    ContextTypeInfo typeInfo(nano);
    QCOMPARE(typeInfo.name(), QString("double"));
    QCOMPARE(typeInfo.parameterDoc("min"), QString("Minimum value"));
    QCOMPARE(typeInfo.parameterDoc("max"), QString("Maximum value"));
    QCOMPARE(typeInfo.parameters().size(), 2);
    QCOMPARE(typeInfo.doc(), QString("A double value within the given limits."));
}

void ContextTypeInfoUnitTest::parseCustomDoubleType()
{
    NanoXml nano(LOCAL_FILE("custom-double.xml"));
    QCOMPARE(nano.didFail(), false);

    ContextTypeInfo typeInfo(nano);
    QCOMPARE(typeInfo.name(), QString("custom-double"));
    QCOMPARE(typeInfo.parameterIntValue("min"), 1);
    QCOMPARE(typeInfo.parameterIntValue("max"), 666);
    QCOMPARE(typeInfo.parameterDoc("min"), QString("Minimum value"));
    QCOMPARE(typeInfo.parameterDoc("max"), QString("Maximum value"));
    QCOMPARE(typeInfo.doc(), QString("A double value that represents the level of hell in you."));
}

void ContextTypeInfoUnitTest::parseUniformList()
{
    NanoXml nano(LOCAL_FILE("uniform-list.xml"));
    QCOMPARE(nano.didFail(), false);

    ContextUniformListTypeInfo listInfo(nano);
    QCOMPARE(listInfo.name(), QString("uniform-list"));
    ContextTypeInfo elementTypeInfo = listInfo.elementType();
    QCOMPARE(elementTypeInfo.name(), QString("double"));
}

#include "contexttypeinfounittest.moc"
QTEST_MAIN(ContextTypeInfoUnitTest);
