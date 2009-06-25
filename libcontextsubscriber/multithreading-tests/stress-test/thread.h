#ifndef STRESS_H
#define STRESS_H

#include <contextproperty.h>

#include <QThread>
#include <QDebug>
#include <QTimer>

#define NUM_TESTS 6


class ValueChecker : public QObject
{
    Q_OBJECT
    ContextProperty* property;

public:
    ValueChecker(ContextProperty* cp, QObject* parent) : QObject(parent), property(cp) {
        connect(property, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
    }

public slots:
    void onValueChanged() {
        qDebug() << "VALUE:" << property->value();
        // check value somehow XXX
    }
};


class Thread : public QThread
{
    Q_OBJECT

public:
    Thread(const int task, const QString& name) : task(task), name(name) {};
    static int value1;

protected:
    void run() {
        // create property do something and delete it
        ContextProperty* cp = new ContextProperty(name);
        switch (task) {
            case 1: // get value
            cp->waitForSubscription();
            qDebug() << "** value =" << cp->value();
            break;

            case 2: // wait 0-2000 msec before deleting
            msleep(qrand()%2000);
            break;

            case 3: // unsubscribe
            cp->unsubscribe();
            break;

            case 4: // unsubscribe and wait a bit
            cp->unsubscribe();
            msleep(qrand()%2000);
            break;

            case 5: // unsubscribe and subscribe
            cp->unsubscribe();
            cp->subscribe();
            break;

            case 6: // unsubscribe, wait a bit then subscribe
            cp->unsubscribe();
            msleep(qrand()%2000);
            cp->subscribe();
            msleep(qrand()%2000);
            break;

            default:; // just create and delete
        }
        delete(cp);
        exit();
    }

private:
    int task;
    QString name;

};


class TestRunner : public QObject
{
    Q_OBJECT

private:
   int maxThreads;
   QSet<Thread*> threads;
   QString propertyName;
   int count;

public:
    TestRunner(const int maxThreads, const QString& propertyName) :
        maxThreads(maxThreads), propertyName(propertyName), count(0) {
//        QTimer::singleShot(100, this, SLOT(start()));
        start();
    }

    void addThread() {
        int task = qrand() % NUM_TESTS;
        qDebug() << "** starting task: " << task << "/" << ++count;
        Thread* t = new Thread(task, propertyName);
        QObject::connect(t, SIGNAL(finished()), this, SLOT(onThreadFinished()));
        threads.insert(t);
        t->start();
    }

public slots:
    void start() {
        for (int i = 0; i < maxThreads; i++)
            addThread();
    }

    void onThreadFinished() {
        Thread* t = (Thread*) QObject::sender();
        if (threads.remove(t)) {
            delete t;
            addThread();
        }
    }
};

#endif
