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
    void doubleType();
    void customDoubleType();
    void uniformList();
};

void ContextTypeInfoUnitTest::basicTypes()
{
    QCOMPARE(ContextTypeInfo::boolType().name(), QString("bool"));
    QCOMPARE(ContextTypeInfo::stringType().name(), QString("string"));
    QCOMPARE(ContextTypeInfo::int64Type().name(), QString("int64"));
}

void ContextTypeInfoUnitTest::doubleType()
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

void ContextTypeInfoUnitTest::customDoubleType()
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

void ContextTypeInfoUnitTest::uniformList()
{
    NanoXml nano(LOCAL_FILE("uniform-list.xml"));
    QCOMPARE(nano.didFail(), false);

    ContextUniformListTypeInfo listInfo(nano);
    QCOMPARE(listInfo.name(), QString("uniform-list"));
    ContextTypeInfo elementTypeInfo = listInfo.elementType();
    QCOMPARE(elementTypeInfo.name(), QString("double"));
    QCOMPARE(elementTypeInfo.parameterIntValue("min"), 0);
    QCOMPARE(elementTypeInfo.parameterIntValue("max"), 10);
    QCOMPARE(elementTypeInfo.doc(), QString("A double value within the given limits."));
}

#include "contexttypeinfounittest.moc"
QTEST_MAIN(ContextTypeInfoUnitTest);
