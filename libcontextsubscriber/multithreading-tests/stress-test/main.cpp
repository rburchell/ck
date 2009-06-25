#include "thread.h"
#include <QCoreApplication>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    int maxTasks = 1;
    if (argc == 2 && atoi(argv[1]) > 0)
        maxTasks =  atoi(argv[1]);

    TestRunner tests(maxTasks, "test.int");

    return app.exec();
}
