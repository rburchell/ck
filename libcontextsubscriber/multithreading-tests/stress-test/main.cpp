#include "thread.h"
#include <QCoreApplication>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    int maxTasks = 0;
    int maxTasks2 = 0;

    if (argc > 1 && atoi(argv[1]) > 0)
        maxTasks =  atoi(argv[1]);
    if (argc > 2 && atoi(argv[2]) > 0)
        maxTasks2 =  atoi(argv[2]);

    TestRunner tests(maxTasks, "test.int");
    TestRunner tests2(maxTasks2, "test2.int");

    return app.exec();
}
