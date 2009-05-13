#include <QtTest/QtTest>
#include <QtCore>
#include "contextpropertyinfo.h"
#include "contextpropertyinfounittest.h"

void ContextPropertyInfoUnitTest::checkKeyTypeChanging()
{
    // Create initial state
    QFile::remove("xml/providers.xml");
    QFile::copy("xml/providers2v1.xml.src", "xml/providers.xml");

    ContextPropertyInfo prop("Battery.LowBattery");
    QCOMPARE(prop.type(), QString("TRUTH"));
    
    QSignalSpy spy(&prop, SIGNAL(typeChanged(QString)));

    QFile::remove("xml/providers.xml");
    QFile::copy("xml/providers2v2.xml.src", "xml/providers.xml");
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
    QFile::remove("xml/providers.xml");
    QFile::copy("xml/providers3v1.xml.src", "xml/providers.xml");

    ContextPropertyInfo prop("Battery.LowBattery");
    QVERIFY(prop.type() != "");
    QVERIFY(prop.doc() != "");
    
    QFile::remove("xml/providers.xml");
    QFile::copy("xml/providers3v2.xml.src", "xml/providers.xml");
    QTest::qWait(500);

    QVERIFY(prop.type() == "");
    QVERIFY(prop.doc() == "");
}

void ContextPropertyInfoUnitTest::cleanupTestCase()
{
    QFile::remove("xml/providers.xml");
}

#include "moc_contextpropertyinfounittest_cpp.cpp"
