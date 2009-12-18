/*
 * Copyright (C) 2008,2009 Nokia Corporation.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#include <QObject>
#include <QtTest/QtTest>

#include "duration.h" // Class to be tested

class DurationUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();

    // Tests
    void toStringTest_data();
    void toStringTest();
    void accessorsTest();
    void fromStringTest_data();
    void fromStringTest();
    void toNanoSecsTest_data();
    void toNanoSecsTest();
    void isDurationTest_data();
    void isDurationTest();
};

void DurationUnitTest::init()
{
}

void DurationUnitTest::toStringTest_data()
{
    QTest::addColumn<quint64>("string");
    QTest::addColumn<QString>("result");
    QTest::newRow("fulldate") << Q_UINT64_C(97959392000000000)
                              << QString("3Y 5W 3D 18H 56M 32S");
    QTest::newRow("oneyear") << Q_UINT64_C(31536000000000000)
                             << QString("1Y");
    QTest::newRow("allbutyear") << Q_UINT64_C(31532459000000000)
                                << QString("52W 23H 59S");
    QTest::newRow("dhms") << Q_UINT64_C(601259000000000)
                          << QString("6D 23H 59S");
    QTest::newRow("hms") << Q_UINT64_C(14410000000000)
                         << QString("4H 10S");
    QTest::newRow("ms") << Q_UINT64_C(3565000000000)
                        << QString("59M 25S");
    QTest::newRow("onlysec") << Q_UINT64_C(25000000000)
                             << QString("25S");
    QTest::newRow("conversiondefinition") << Q_UINT64_C(63072000000000000)
                             << QString("2Y");
    QTest::newRow("conversiondefinition1") << Q_UINT64_C(31622400000000000)
                                 << QString("1Y 1D");
    QTest::newRow("conversiondefinition2") << Q_UINT64_C(31536001000000000)
                                 << QString("1Y 1S");
}

void DurationUnitTest::toStringTest()
{
    QFETCH(quint64, string);
    QFETCH(QString, result);
    Duration duration (string);
    QCOMPARE(duration.toString(), result);
}

void DurationUnitTest::accessorsTest()
{
    Duration duration (Q_UINT64_C(97959392000000000));
    QCOMPARE(duration.seconds(), 32);
    QCOMPARE(duration.minutes(), 56);
    QCOMPARE(duration.hours(), 18);
    QCOMPARE(duration.days(), 3);
    QCOMPARE(duration.weeks(), 5);
    QCOMPARE(duration.years(), 3);
}

void DurationUnitTest::fromStringTest_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<quint64>("result");
    QTest::newRow("fulldate") << QString("3Y 5W 3D 18H 56M 32S")
                              << Q_UINT64_C(97959392000000000);
    QTest::newRow("oneyear") << QString("1Y 0W 0D 0H 0M 0S")
                              << Q_UINT64_C(31536000000000000);
    QTest::newRow("allbutyear") << QString("52W 23H 59S")
                                << Q_UINT64_C(31532459000000000);
    QTest::newRow("dhms") << QString("6D 23H 59S")
                          << Q_UINT64_C(601259000000000);
    QTest::newRow("hms") << QString("4H 10S")
                         << Q_UINT64_C(14410000000000);
    QTest::newRow("ms") << QString("59M 25S")
                        << Q_UINT64_C(3565000000000);
    QTest::newRow("onlysec") << QString("25S")
                             << Q_UINT64_C(25000000000);
    QTest::newRow("conversiondefinition") << QString("730D")
                             << Q_UINT64_C(63072000000000000);
    QTest::newRow("conversiondefinition2") << QString("366D")
                             << Q_UINT64_C(31622400000000000);
    QTest::newRow("conversiondefinition3") << QString("1Y 1D")
                             << Q_UINT64_C(31622400000000000);
    QTest::newRow("conversiondefinition4") << QString("1Y 1S")
                             << Q_UINT64_C(31536001000000000);
    QTest::newRow("conversiondefinition5") << QString("52W 1D ")
                             << Q_UINT64_C(31536000000000000);
    QTest::newRow("emptystring") << QString("")
                             << Q_UINT64_C(0);
}

void DurationUnitTest::fromStringTest()
{
    QFETCH(QString, string);
    QFETCH(quint64, result);
    QCOMPARE((Duration(string)).toNanoSeconds(), result);
}

void DurationUnitTest::toNanoSecsTest_data()
{
    QTest::addColumn<quint64>("string");
    QTest::addColumn<quint64>("result");
    QTest::newRow("fulldate") << Q_UINT64_C(97959392000000000)
                              << Q_UINT64_C(97959392000000000);
    QTest::newRow("oneyear") << Q_UINT64_C(31536000000000000)
                             << Q_UINT64_C(31536000000000000);
    QTest::newRow("allbutyear") << Q_UINT64_C(31532459000000000)
                                << Q_UINT64_C(31532459000000000);
    QTest::newRow("dhms") << Q_UINT64_C(601259000000000)
                          << Q_UINT64_C(601259000000000);
    QTest::newRow("hms") << Q_UINT64_C(14410000000000)
                         << Q_UINT64_C(14410000000000);
    QTest::newRow("ms") << Q_UINT64_C(3565000000000)
                        << Q_UINT64_C(3565000000000);
    QTest::newRow("onlysec") << Q_UINT64_C(25000000000)
                             << Q_UINT64_C(25000000000);
    QTest::newRow("conversiondefinition") << Q_UINT64_C(63072000000000000)
                                << Q_UINT64_C(63072000000000000);
    QTest::newRow("conversiondefinition1") << Q_UINT64_C(31622400000000000)
                                 << Q_UINT64_C(31622400000000000);
    QTest::newRow("conversiondefinition2") << Q_UINT64_C(31536001000000000)
                                 << Q_UINT64_C(31536001000000000);
}

void DurationUnitTest::toNanoSecsTest()
{
    QFETCH(quint64, string);
    QFETCH(quint64, result);
    Duration duration (string);
    QCOMPARE(duration.toNanoSeconds(), result);
}

void DurationUnitTest::isDurationTest_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<bool>("result");
    QTest::newRow("fulldate") << QString("3Y 5W 3D 18H 56M 32S")
                              << true;
    QTest::newRow("emptystring") << QString("")
                             << false;
    QTest::newRow("space") << QString(" ")
                             << false;
    QTest::newRow("invalid") << QString("foobar")
                             << false;
}

void DurationUnitTest::isDurationTest()
{
    QFETCH(QString, string);
    QFETCH(bool, result);
    QCOMPARE(Duration::isDuration(string), result);
}


QTEST_MAIN(DurationUnitTest);
#include "testduration.moc"
