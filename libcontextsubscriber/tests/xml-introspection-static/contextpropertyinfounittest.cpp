#include <QtTest/QtTest>
#include <QtCore>
#include "contextpropertyinfo.h"
#include "contextpropertyinfounittest.h"

void ContextPropertyInfoUnitTest::checkLowBAttery()
{
    ContextPropertyInfo prop("Context.Battery.LowBattery");

    QCOMPARE(prop.key(), QString("Context.Battery.LowBattery"));
    QCOMPARE(prop.doc(), QString("This is true when battery is low"));
    QCOMPARE(prop.type(), QString("TRUTH"));
    QCOMPARE(prop.providerDBusName(), QString("org.freedesktop.ContextKit.contextd1"));
}

#include "moc_contextpropertyinfounittest_cpp.cpp"
