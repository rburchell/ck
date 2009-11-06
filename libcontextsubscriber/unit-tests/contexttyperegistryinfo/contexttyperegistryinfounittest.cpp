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
    void string();
};

void ContextTypeRegistryInfoUnitTest::initTestCase()
{
    utilCopyLocalAtomically("core.types.src", "core.types");
    utilSetEnv("CONTEXT_TYPES", "./");
    utilSetEnv("CONTEXT_CORE_TYPES", "core.types");
    registry = ContextTypeRegistryInfo::instance();
}

void ContextTypeRegistryInfoUnitTest::string()
{
    NanoTree stringDef = registry->stringType();
    QCOMPARE(stringDef.keyValue("name").toString(), QString("string"));
    QCOMPARE(stringDef.keyValue("doc").toString(), QString("A string."));
}

void ContextTypeRegistryInfoUnitTest::cleanupTestCase()
{
     QFile::remove("core.types");
}

#include "contexttyperegistryinfounittest.moc"
QTEST_MAIN(ContextTypeRegistryInfoUnitTest);
