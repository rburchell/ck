#include <QtTest/QtTest>
#include <QtCore>
#include "contextpropertyinfo.h"
#include "contextpropertyinfounittest.h"

void ContextPropertyInfoUnitTest::checkKeyTypeChanging()
{
    // Create initial state
    QFile::remove("providers.xml");
    QFile::copy("providers2v1.xml.src", "providers.xml");

    ContextPropertyInfo prop("Context.Battery.LowBattery");
    QCOMPARE(prop.type(), QString("TRUTH"));
    
    QSignalSpy spy(&prop, SIGNAL(typeChanged(QString)));

    QFile::remove("providers.xml");
    QFile::copy("providers2v2.xml.src", "providers.xml");
    QTest::qWait(500);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toString(), QString("INT"));

    // Now just make sure that the new val is ok too
    QCOMPARE(prop.type(), QString("INT"));
}

void ContextPropertyInfoUnitTest::checkKeyRemoval()
{
    // Create initial state
    QFile::remove("providers.xml");
    QFile::copy("providers3v1.xml.src", "providers.xml");

    ContextPropertyInfo prop("Context.Battery.LowBattery");
    QVERIFY(prop.type() != "");
    QVERIFY(prop.doc() != "");
    
    QFile::remove("providers.xml");
    QFile::copy("providers3v2.xml.src", "providers.xml");
    QTest::qWait(500);

    QVERIFY(prop.type() == "");
    QVERIFY(prop.doc() == "");
}

void ContextPropertyInfoUnitTest::cleanupTestCase()
{
    QFile::remove("providers.xml");
}

#include "moc_contextpropertyinfounittest_cpp.cpp"
