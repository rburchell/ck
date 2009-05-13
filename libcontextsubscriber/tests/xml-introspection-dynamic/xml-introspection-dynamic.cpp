#include <QtTest/QtTest>
#include <QtCore>
#include "contextregistryinfo.h"
#include "contextregistryinfounittest.h"
#include "contextpropertyinfounittest.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    setenv("CONTEXT_PROVIDERS", "xml/", 0);

    // Force the xml backend before anything else happens...
    ContextRegistryInfo::instance("xml");
    
    ContextRegistryInfoUnitTest test1;
    QTest::qExec(&test1);

    ContextPropertyInfoUnitTest test2;
    QTest::qExec(&test2);

    return 0;
}
