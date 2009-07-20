#include <QCoreApplication>
#include "test-cli.h"

ContextD::ContextD() : QObject(NULL), batStat(true)
{
    QStringList keys;
    keys.append("Battery.OnBattery");
    keys.append("Battery.ChargePercentage");

    Context::initService(QDBusConnection::SessionBus, 
                         "org.freedesktop.ContextKit.contextd", 
                         keys);
                         
    group = new SignalGrouper(keys);

    onBattery = new Context("Battery.OnBattery");
    chargePercentage = new Context("Battery.ChargePercentage");

    sconnect(group, SIGNAL(firstSubscriberAppeared()),
            this, SLOT(onFirstSubscriberAppeared()));

    sconnect(group, SIGNAL(lastSubscriberDisappeared()),
            this, SLOT(onLastSubscriberDisappeared()));

    sconnect(&timer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));
}

void ContextD::onFirstSubscriberAppeared()
{
    contextDebug() << "First subscriber! Starting service.";
    onBattery->set(batStat);
    chargePercentage->set((batStat) ? 50 : 100);
    timer.start(3000);
}

void ContextD::onLastSubscriberDisappeared()
{
    contextDebug() << "last subscriber! Stopping service.";
    timer.stop();
}

void ContextD::onTimeout()
{
    contextDebug() << "Timeout! Flipping OnBattery and altering ChargePercentage";
    batStat = ! batStat;
    onBattery->set(batStat);
    chargePercentage->set((batStat) ? 50 : 100);
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    
    ContextD contextd;

    return app.exec();
}

