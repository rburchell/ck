#include <QtTest/QtTest>
#include <QtCore>
#include "contextregistryinfo.h"
#include "contextregistryinfounittest.h"

void ContextRegistryInfoUnitTest::initTestCase()
{
    context = ContextRegistryInfo::instance();
    QVERIFY(context != NULL);
}

void ContextRegistryInfoUnitTest::basicChange()
{
    // Setup state
    QFile::remove("xml/providers.xml");
    QFile::copy("xml/providers1v1.xml.src", "xml/providers.xml");

    QSignalSpy spy(context, SIGNAL(keysChanged(QStringList)));

    QFile::remove("xml/providers.xml");
    QFile::copy("xml/providers1v2.xml.src", "xml/providers.xml");
    QTest::qWait(500);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toList().size(), 3);

    // Now just make sure that the new key is availible
    QStringList newKeys = context->listKeys();
    QVERIFY(newKeys.contains("Context.Battery.AboutToExplode"));
}

void ContextRegistryInfoUnitTest::cleanupTestCase()
{
    QFile::remove("xml/providers.xml");
}
    
#include "moc_contextregistryinfounittest_cpp.cpp"
