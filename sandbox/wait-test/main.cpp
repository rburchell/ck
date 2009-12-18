#include <QTimer>
#include <QDebug>
#include <QWaitCondition>
#include <QMutex>
#include <QCoreApplication>

int i=0;

class MyTimer:public QTimer
{
    Q_OBJECT;
public:
    MyTimer()
        {
            connect(this, SIGNAL(timeout()),
                    this, SLOT(timedout()));
            start(10000);
            setSingleShot(true);
        }
public Q_SLOTS:
    void timedout()
        {
            qDebug() << "timed out, wake up everyone";
            i=1;
        }
};

class MyTimerWaiter:public QTimer
{
    Q_OBJECT;
public:
    MyTimerWaiter()
        {
            connect(this, SIGNAL(timeout()),
                    this, SLOT(timedout()));
            start(0);
        }
public Q_SLOTS:
    void timedout()
        {
            qDebug() << "timed out, so starting to wait";
            while(i == 0) {
                qDebug() << "I'm entering processEvents";
                QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
                qDebug() << "I've been exited from processEvents";
                usleep(1000000);
            }

            qDebug() << "I've been awakened";
            exit(1);
        }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

//    MyTimerWaiter waiter;
    MyTimer timer;

    while(i == 0) {
        qDebug() << "I'm entering processEvents";
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
        qDebug() << "I've been exited from processEvents";
    }

    qDebug() << "NOT starting the main loop";
    return 0;

    return app.exec();
}

#include "main.moc"
