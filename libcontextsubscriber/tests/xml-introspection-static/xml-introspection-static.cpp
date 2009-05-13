#include <QtTest/QtTest>
#include <QtCore>
#include "contextregistryinfo.h"
#include "contextpropertyinfounittest.h"
#include "contextregistryinfounittest.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    setenv("CONTEXT_PROVIDERS", "./", 0);

    // Force the xml backend before anything else happens...
    ContextRegistryInfo::instance("xml");
    
    ContextRegistryInfoUnitTest test1;
    QTest::qExec(&test1);

    ContextPropertyInfoUnitTest test2;
    QTest::qExec(&test2);

    return 0;
}
