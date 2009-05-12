#include <QtTest/QtTest>
#include <QtCore>
#include "cdbwriterunittest.h"

void CDBWriterUnitTest::basicCreation()
{
    CDBWriter writer("test.cdb");
    writer.add("KEYS", "KEYSValue1");
    writer.add("KEYS", "KEYSValue2");
    writer.add("KEYS", "KEYSValue3");

    writer.add("KEY1", "KEY1Value");
    writer.close();
}

#include "moc_cdbwriterunittest_cpp.cpp"
