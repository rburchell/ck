#include <QObject>

class QProcess;

#include "context.h"

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
    bool isReadyToRead;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testGetSubscriber();
    void testGetSubscriberTwice();

    void subscribeReturnValueForUnknownProperty();
    void subscribeReturnValueForKnownProperty();
    void subscribeReturnValueForInvalidProperty();

    void subscriberNotifications();

public:
    SubscriptionTests();

public slots:
    void readStandardOutput();

private:
    QString writeToClient(const char* input);

private:
    bool clientStarted;
};

} // end namespace
