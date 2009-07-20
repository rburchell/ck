#include <QStringList>
#include <QObject>
#include "context.h"
#include "signalgrouper.h"
#include "sconnect.h"
#include "logging.h"
#include <QDebug>
#include <QTimer>

class ContextD : public QObject
{
    Q_OBJECT

public:
    ContextD();

public slots:
    void onFirstSubscriberAppeared();
    void onLastSubscriberDisappeared();
    void onTimeout();

private:
    SignalGrouper *group;
    Context *onBattery;
    Context *chargePercentage;
    QTimer timer;
    bool batStat;
};

