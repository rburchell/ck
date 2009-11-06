/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
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
#include "contexttyperegistryinfo.h"
#include "fileutils.h"

/* ContextRegistryInfoUnitTest */

class ContextTypeRegistryInfoUnitTest : public QObject
{
    Q_OBJECT

private:
    ContextTypeRegistryInfo *registry;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void stringDef();
    void doubleDef();
    void int32Def();
};

void ContextTypeRegistryInfoUnitTest::initTestCase()
{
    utilCopyLocalAtomically("core.types.src", "core.types");
    utilSetEnv("CONTEXT_TYPES", "./");
    utilSetEnv("CONTEXT_CORE_TYPES", "core.types");
    registry = ContextTypeRegistryInfo::instance();
}

void ContextTypeRegistryInfoUnitTest::stringDef()
{
    NanoTree def = registry->stringType();
    QCOMPARE(def.keyValue("name").toString(), QString("string"));
    QCOMPARE(def.keyValue("doc").toString(), QString("A string."));

    NanoTree oldDef = registry->typeDefinitionForName("STRING");
    QCOMPARE(oldDef.keyValue("name").toString(), QString("string"));
}

void ContextTypeRegistryInfoUnitTest::doubleDef()
{
    NanoTree def = registry->doubleType();
    QCOMPARE(def.keyValue("name").toString(), QString("double"));
    QCOMPARE(def.keyValue("doc").toString(), QString("A double."));

    NanoTree oldDef = registry->typeDefinitionForName("DOUBLE");
    QCOMPARE(oldDef.keyValue("name").toString(), QString("double"));
}

void ContextTypeRegistryInfoUnitTest::int32Def()
{
    NanoTree def = registry->int32Type();
    QCOMPARE(def.keyValue("name").toString(), QString("int32"));
    QCOMPARE(def.keyValue("doc").toString(), QString("An integer."));

    NanoTree oldDef = registry->typeDefinitionForName("INT");
    QCOMPARE(oldDef.keyValue("name").toString(), QString("int32"));
}

void ContextTypeRegistryInfoUnitTest::cleanupTestCase()
{
     QFile::remove("core.types");
}

#include "contexttyperegistryinfounittest.moc"
QTEST_MAIN(ContextTypeRegistryInfoUnitTest);
