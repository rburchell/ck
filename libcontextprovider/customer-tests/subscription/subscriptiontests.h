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
    Property *intItem;
    Property *boolItem;
    Property *doubleItem;
    Property *stringItem;
    QProcess *client;
    QString clientName;
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

public slots:
    void readStandardOutput();

private:
    QString writeToClient(const char* input);

private:
    bool clientStarted;
};

} // end namespace
