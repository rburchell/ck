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
#include "boolsysfspooler.h"

class BoolSysFsPoolerUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void basic();
    void fileMissing();
    void incorrectData();
};

void BoolSysFsPoolerUnitTest::basic()
{
    BoolSysFsPooler pooler("input1.txt");
    QCOMPARE(pooler.getState(), BoolSysFsPooler::TriStateTrue);
}

void BoolSysFsPoolerUnitTest::fileMissing()
{
    BoolSysFsPooler pooler("does-not-exist.txt");
    QCOMPARE(pooler.getState(), BoolSysFsPooler::TriStateUnknown);
}

void BoolSysFsPoolerUnitTest::incorrectData()
{
    BoolSysFsPooler pooler("input2.txt");
    QCOMPARE(pooler.getState(), BoolSysFsPooler::TriStateUnknown);
}

#include "boolsysfspoolerunittest.moc"
QTEST_MAIN(BoolSysFsPoolerUnitTest);
