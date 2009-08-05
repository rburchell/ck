#include "commandwatcher.h"
#include "sconnect.h"
#include <QFile>
#include <QSocketNotifier>
#include <QStringList>
#include <QMap>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusObjectPath>
#include <QCoreApplication>
#include <QDebug>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <QtDBus/QtDBus>

CommandWatcher::CommandWatcher(int commandfd, QObject *parent) :
    QObject(parent), commandfd(commandfd), out(stdout)
{
    fcntl(commandfd, F_SETFL, O_NONBLOCK);
    commandNotifier = new QSocketNotifier(commandfd, QSocketNotifier::Read, this);
    sconnect(commandNotifier, SIGNAL(activated(int)), this, SLOT(onActivated()));
    help();
}

void CommandWatcher::onActivated()
{
    // read all available input to commandBuffer
    static QByteArray commandBuffer = "";
    static char buf[1024];
    int readSize;
    while ((readSize = read(commandfd, &buf, 1024)) > 0)
        commandBuffer += QByteArray(buf, readSize);

    // handle all available whole input lines as commands
    int nextSeparator;
    while ((nextSeparator = commandBuffer.indexOf('\n')) != -1) {
        // split lines to separate commands by semicolons
        QStringList commands = QString::fromUtf8(commandBuffer.constData()).left(nextSeparator).split(";");
        foreach (QString command, commands)
            interpret(command.trimmed());
        commandBuffer.remove(0, nextSeparator + 1);
    }

    if (readSize == 0) // EOF
        QCoreApplication::exit(0);
}

void CommandWatcher::help()
{
        qDebug() << "Available commands:";
        qDebug() << "  getsubscriber BUSTYPE BUSNAME   - execute GetSubscriber over DBus";
        qDebug() << "  subscribe BUSNAME KEY...        - subscribe to keys for a known BUSNAME";
        qDebug() << "  unsubscribe BUSNAME KEY...      - unsubscribe from keys for a known BUSNAME";
        qDebug() << "  resetsignalstatus               - forget any previously received Changed signals";
        qDebug() << "  waitforchanged TIMEOUT          - wait until the Changed signal arrives over DBus";
        qDebug() << "Any prefix of a command can be used as an abbreviation";
}

void CommandWatcher::interpret(const QString& command)
{
    if (command == "") {
        help();
    } else {
        QStringList args = command.split(" ");
        QString commandName = args[0];
        args.pop_front();

        if (QString("getsubscriber").startsWith(commandName)) {
            if (args.size() == 2) {
                // Create the DBus connection to the correct bus (session or system)
                QString busType = args.at(0);
                if (busType == "session" || busType == "system") {
                    // Store the bus type
                    QString busName = args.at(1);
                    connectionTypes.insert(busName, busType);
                    callGetSubscriber(getConnection(busType), busName);
                }
                else {
                    // Print the error message to output (so that the test program waiting for input won't block)
                    out << "Error: invalid bus type " << busType << endl;
                }
            }
            else {
                out << "Error: wrong number of parameters" << endl;
            }
        } else if (QString("subscribe").startsWith(commandName)) {
            if (args.size() >= 1) {
                QString busName = args[0];
                args.pop_front();
                callSubscribe(busName, args);
            }
            else
                out << "Error: wrong number of parameters" << endl;
        } else if (QString("unsubscribe").startsWith(commandName)) {
            if (args.size() >= 1) {
                QString busName = args[0];
                args.pop_front();
                callUnsubscribe(busName, args);
            }
            else
                out << "Error: wrong number of parameters" << endl;
        } else if (QString("resetsignalstatus").startsWith(commandName)) {
            resetSignalStatus();
        } else if (QString("waitforchanged").startsWith(commandName)) {
            if (args.size() == 2) {
                bool conversionOk = false;
                int timeout = args.at(0).toInt(&conversionOk);
                if (! conversionOk) {
                    out << "Error: parameter " << args.at(0) << " cannot be converted to int" << endl;
                    return;
                }
                waitForChanged(timeout);
            }
            else {
                out << "Error: wrong number of parameters" << endl;
            }
        }
    }
}

void CommandWatcher::callGetSubscriber(QDBusConnection connection, const QString& busName)
{
    // Call GetSubscriber synchronously
    QDBusInterface manager(busName, "/org/freedesktop/ContextKit/Manager",
                           "org.freedesktop.ContextKit.Manager", connection);
    QDBusReply<QDBusObjectPath> reply = manager.call("GetSubscriber");
    if (reply.isValid()) {
        // Store the subscriber path
        QString subscriberPath = reply.value().path();
        subscriberPaths.insert(busName, subscriberPath);
        // And print it out
        out << "GetSubscriber returned " << subscriberPath << endl;
    }
    else {
        out << "GetSubscriber error: invalid reply" << endl;
        // Nullify the subscriber path so that we don't use it accidentally later
        subscriberPaths.remove(busName);
    }
}

void CommandWatcher::callSubscribe(const QString& busName, const QStringList& args)
{
    QString subscriberPath = subscriberPaths[busName];
    if (subscriberPath != "") {
        // We have a proper subscriber path

        QDBusConnection connection = getConnection(connectionTypes[busName]);

        QDBusInterface subscriber(busName, subscriberPath,
                                  "org.freedesktop.ContextKit.Subscriber", connection);

        // Call Subscribe "synchronously", by first
        // constructing an asynchronous call and then waiting
        // for it to be finished.  For some reason, it seems
        // that extracting the QMap<QString, QVariant> is
        // tricky without using QDBusPendingReply.
        QDBusPendingReply<QMap<QString, QVariant>, QStringList> reply = subscriber.asyncCall("Subscribe", args);
        reply.waitForFinished();
        if (!reply.isValid()) {
            out << "Subscribe error: invalid reply" << endl;
            return;
        }

        QMap<QString, QVariant> knownValues = reply.argumentAt<0>();
        QStringList unknownKeys = reply.argumentAt<1>();
        unknownKeys.sort();

        QStringList knownKeys(knownValues.keys());
        knownKeys.sort();

        out << "Known keys: ";
        foreach (const QString& key, knownKeys) {
            out << key << " ";
        }

        out << " Unknown keys: ";
        foreach (const QString& key, unknownKeys) {
            out << key << " ";
        }
        out << endl;
    }
    else {
        out << "Subscribe error: we don't have a subscriber for bus name " << busName << endl;
    }
}

void CommandWatcher::callUnsubscribe(const QString& busName, const QStringList& args)
{
    QString subscriberPath = subscriberPaths[busName];
    if (subscriberPath != "") {
        // We have a proper subscriber path

        QDBusConnection connection = getConnection(connectionTypes[busName]);

        QDBusInterface subscriber(busName, subscriberPath,
                                  "org.freedesktop.ContextKit.Subscriber", connection);
        // Call Unsubscribe synchronously
        subscriber.call("Unsubscribe", args);
    }
    else {
        out << "Unsubscribe error: we don't have a subscriber for bus name " << busName << endl;
    }
}

void CommandWatcher::resetSignalStatus()
{
    changedSignalReceived = false;
}

void CommandWatcher::waitForChanged(int timeout)
{
    QTime t;
    t.start();
    while (changedSignalReceived == false && t.elapsed() < timeout) {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    if (changedSignalReceived) {
        out << "Changed signal received" << endl;
    }
    else {
        out << "Timeout" << endl;
    }
}

void CommandWatcher::onChanged()
{
    changedSignalReceived = true;
}

QDBusConnection CommandWatcher::getConnection(const QString& busType)
{
    if (busType == "system")
        return QDBusConnection::systemBus();
    return QDBusConnection::sessionBus();
}
