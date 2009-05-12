#include <QtTest/QtTest>
#include <QtCore>
#include "cdbreaderunittest.h"

void CDBReaderUnitTest::basicCreation()
{
    CDBReader reader("test.cdb");

    QCOMPARE(reader.valueForKey("KEY1"), QString("KEY1Value"));

    QStringList list = reader.valuesForKey("KEYS");
    QCOMPARE(list.size(), 3);

    QCOMPARE(list.at(0), QString("KEYSValue1"));
    QCOMPARE(list.at(1), QString("KEYSValue2"));
    QCOMPARE(list.at(2), QString("KEYSValue3"));

    reader.close();
}

void CDBReaderUnitTest::cleanupTestCase()
{
    QFile::remove("test.cdb");
}


#include "moc_cdbreaderunittest_cpp.cpp"
