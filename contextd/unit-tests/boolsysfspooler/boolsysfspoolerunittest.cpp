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
#include <QDir>
#include <QTest>
#include "boolsysfspooler.h"

#define DEFAULT_WAIT_PERIOD 100

void utilWriteToFile(const char *file, const char *data, int waitPeriod = DEFAULT_WAIT_PERIOD)
{
    QFile f(file);
    f.open(QIODevice::WriteOnly | QIODevice::Unbuffered);
    f.write(data);
    f.flush();
    f.close();

    if (waitPeriod > 0)
        QTest::qWait(waitPeriod);
}

class BoolSysFsPoolerUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void basic();
    void fileMissing();
    void incorrectData();
    void changing();
};

void BoolSysFsPoolerUnitTest::basic()
{
    BoolSysFsPooler pooler("input1.txt");
    QSignalSpy spy(&pooler, SIGNAL(stateChanged(TriState)));
    QCOMPARE(pooler.getState(), BoolSysFsPooler::TriStateTrue);
    QCOMPARE(spy.count(), 0);
}

void BoolSysFsPoolerUnitTest::fileMissing()
{
    BoolSysFsPooler pooler("does-not-exist.txt");
    QSignalSpy spy(&pooler, SIGNAL(stateChanged(TriState)));
    QCOMPARE(pooler.getState(), BoolSysFsPooler::TriStateUnknown);
    QCOMPARE(spy.count(), 0);
}

void BoolSysFsPoolerUnitTest::incorrectData()
{
    BoolSysFsPooler pooler("input2.txt");
    QSignalSpy spy(&pooler, SIGNAL(stateChanged(TriState)));
    QCOMPARE(pooler.getState(), BoolSysFsPooler::TriStateUnknown);
    QCOMPARE(spy.count(), 0);
}

void BoolSysFsPoolerUnitTest::changing()
{
    utilWriteToFile("input3.txt", "0");
    BoolSysFsPooler pooler("input3.txt");
    QSignalSpy spy(&pooler, SIGNAL(stateChanged(TriState)));
    
    QCOMPARE(pooler.getState(), BoolSysFsPooler::TriStateFalse); 

    utilWriteToFile("input3.txt", "1");
    
    QCOMPARE(pooler.getState(), BoolSysFsPooler::TriStateTrue); 
    QCOMPARE(spy.count(), 1);
}

#include "boolsysfspoolerunittest.moc"
QTEST_MAIN(BoolSysFsPoolerUnitTest);
