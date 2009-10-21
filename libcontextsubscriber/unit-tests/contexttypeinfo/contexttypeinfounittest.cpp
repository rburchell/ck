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
    void doubleType();
};

void ContextTypeInfoUnitTest::doubleType()
{
    NanoXml nano(LOCAL_FILE("double.xml"));
    QCOMPARE(nano.didFail(), false);

    ContextTypeInfo typeInfo(nano);
    QCOMPARE(typeInfo.name(), QString("double"));
    QCOMPARE(typeInfo.parameters().at(0).keyValue("doc").toString(), QString("Minimum value"));
    QCOMPARE(typeInfo.parameters().at(1).keyValue("doc").toString(), QString("Maximum value"));
    QCOMPARE(typeInfo.parameters().size(), 2);
    QCOMPARE(typeInfo.parameter("min").keyValue("doc").toString(), QString("Minimum value"));
    QCOMPARE(typeInfo.parameter("max").keyValue("doc").toString(), QString("Maximum value"));
}

#include "contexttypeinfounittest.moc"
QTEST_MAIN(ContextTypeInfoUnitTest);
