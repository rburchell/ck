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

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void stringDef();
    void doubleDef();
    void integerDef();
    void boolDef();
    void registryPath();
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
    AssocTree def = registry->typeDefinitionForName("string");
    QCOMPARE(def.value("name").toString(), QString("string"));
    QCOMPARE(def.value("doc").toString(), QString("A string."));

    AssocTree oldDef = registry->typeDefinitionForName("STRING");
    QCOMPARE(oldDef.value("name").toString(), QString("string"));
}

void ContextTypeRegistryInfoUnitTest::doubleDef()
{
    AssocTree def = registry->typeDefinitionForName("double");
    QCOMPARE(def.value("name").toString(), QString("double"));
    QCOMPARE(def.value("doc").toString(), QString("A double."));

    AssocTree oldDef = registry->typeDefinitionForName("DOUBLE");
    QCOMPARE(oldDef.value("name").toString(), QString("double"));
}

void ContextTypeRegistryInfoUnitTest::integerDef()
{
    AssocTree def = registry->typeDefinitionForName("integer");
    QCOMPARE(def.value("name").toString(), QString("integer"));
    QCOMPARE(def.value("doc").toString(), QString("An integer."));

    AssocTree oldDef = registry->typeDefinitionForName("INT");
    QCOMPARE(oldDef.value("name").toString(), QString("integer"));
}

void ContextTypeRegistryInfoUnitTest::boolDef()
{
    AssocTree def = registry->typeDefinitionForName("bool");
    QCOMPARE(def.value("name").toString(), QString("bool"));
    QCOMPARE(def.value("doc").toString(), QString("A boolean."));

    AssocTree oldDef = registry->typeDefinitionForName("TRUTH");
    QCOMPARE(oldDef.value("name").toString(), QString("bool"));
}

void ContextTypeRegistryInfoUnitTest::registryPath()
{
    QCOMPARE(registry->registryPath(),QString("./"));
}

void ContextTypeRegistryInfoUnitTest::cleanupTestCase()
{
     QFile::remove("core.types");
}

#include "contexttyperegistryinfounittest.moc"
QTEST_MAIN(ContextTypeRegistryInfoUnitTest);
