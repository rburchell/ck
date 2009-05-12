#include <QtTest/QtTest>
#include <QtCore>
#include "cdbwriter.h"

class CDBWriterUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void basicCreation();
};

void CDBWriterUnitTest::basicCreation()
{
    CDBWriter writer("test.cdb");
    writer.add("Key", "Value");
    writer.close();
}

QTEST_MAIN(CDBWriterUnitTest);
#include "moc_cdbwriterunittest_cpp.cpp"
