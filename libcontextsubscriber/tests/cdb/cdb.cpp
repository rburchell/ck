#include <QtTest/QtTest>
#include <QtCore>
#include "cdbwriterunittest.h"
#include "cdbreaderunittest.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
   
    CDBWriterUnitTest test1;
    QTest::qExec(&test1);

    CDBReaderUnitTest test2;
    QTest::qExec(&test2);

    return 0;
}
