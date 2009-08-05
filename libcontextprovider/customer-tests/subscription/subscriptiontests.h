#include "context.h"
#include "sconnect.h"
#include "logging.h"
#include <QStringList>
#include <QObject>
#include <QDebug>
#include <QProcess>

namespace ContextProvider {

class SubscriptionTests : public QObject
{
    Q_OBJECT

private:
    Service service1;
    Property test_int;
    Property test_double;

    Service service2;
    Property test_string;
    Property test_bool;

    QProcess *client;
    QString clientName;
    bool isReadyToRead;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testGetSubscriber();

public:
    SubscriptionTests();

public slots:
    void readStandardOutput();
};

} // end namespace
