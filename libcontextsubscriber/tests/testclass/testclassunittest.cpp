#include <QtTest/QtTest>
#include "testclass.h"

class TestClassUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void basic();
};

void TestClassUnitTest::basic()
{
    TestClass tc;
    QVERIFY(tc.testMe() == true);
}

QTEST_MAIN(TestClassUnitTest);
#include "moc_testclassunittest_cpp.cpp"
