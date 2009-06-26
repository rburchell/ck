#ifndef STRESS_H
#define STRESS_H

#include <contextproperty.h>

#include <time.h>
#include <QThread>
#include <QDebug>

#define NUM_TESTS 8

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
    Thread(const int task, const QString& propertyName) : task(task), propertyName(propertyName) {};

protected:
    void run() {
        // create property do something and delete it
        ContextProperty* cp = new ContextProperty(propertyName);
        int value, realValue;

        switch (task) {
            case 1: // check value
            cp->waitForSubscription();
            value =  cp->value().toInt();
            realValue = time(0);
            if (value != realValue && value != 0) {
                msleep(100); // give it 0.1 sec to arrive
                value =  cp->value().toInt();
                if (value != realValue && value != 0) 
                    qDebug() << "*** value mismatch:" << propertyName << value << realValue;
            }
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

            case 6: // unsubscribe, wait a bit then subscribe and wait some more
            cp->unsubscribe();
            msleep(qrand()%2000);
            cp->subscribe();
            msleep(qrand()%2000);
            break;

            case 7: // subscribe
            cp->subscribe();
            break;

            case 8: // subscribe
            msleep(qrand()%100);
            cp->subscribe();
            msleep(qrand()%100);
            break;

            default:; // just create and delete
        }

        delete(cp);
        exit();
    }

private:
    int task;
    QString propertyName;

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
        for (int i = 0; i < maxThreads; i++)
            addThread();
    }

    void addThread() {
        int task = qrand() % NUM_TESTS;
        qDebug() << "** starting" << propertyName << task << "/" << ++count;
        Thread* t = new Thread(task, propertyName);
        QObject::connect(t, SIGNAL(finished()), this, SLOT(onThreadFinished()));
        threads.insert(t);
        t->start();
    }

public slots:
    void onThreadFinished() {
        Thread* t = (Thread*) QObject::sender();
        if (threads.remove(t)) {
            delete t;
            addThread();
        }
    }
};

#endif
