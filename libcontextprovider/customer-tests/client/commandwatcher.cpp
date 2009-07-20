#include "commandwatcher.h"
#include "sconnect.h"
#include <QTextStream>
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
    QObject(parent), commandfd(commandfd), subscriberPath(""), connection(QDBusConnection::sessionBus())
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
        qDebug() << "  subscribe KEY...                - subscribe to keys";
        qDebug() << "  unsubscribe KEY...              - unsubscribe from keys";
        qDebug() << "Any prefix of a command can be used as an abbreviation";
}

void CommandWatcher::interpret(const QString& command)
{
    QTextStream out(stdout);
    if (command == "") {
        help();
    } else {
        QStringList args = command.split(" ");
        QString commandName = args[0];
        args.pop_front();

        if (args.size() == 0) {
            help();
            return;
        }

        if (QString("getsubscriber").startsWith(commandName)) {
            if (args.size() == 2) {

                // Create the DBus connection to the correct bus (session or system)
                QString busType = args.at(0);
                if (busType == "session") {
                    connection = QDBusConnection::sessionBus();
                }
                else if (busType == "system") {
                    connection = QDBusConnection::systemBus();
                }
                else {
                    // Print the error message to output (so that the test program waiting for input won't block)
                    out << "GetSubscriber error: wrong parameters" << endl;
                    return;
                }
                // Store the bus name
                busName = args.at(1);

                // Call GetSubscriber synchronously
                QDBusInterface manager(busName, "/org/freedesktop/ContextKit/Manager",
                                       "org.freedesktop.ContextKit.Manager", connection);
                QDBusReply<QDBusObjectPath> reply = manager.call("GetSubscriber");
                if (reply.isValid()) {
                    // Store the subscriber path
                    subscriberPath = reply.value().path();
                    // And print it out
                    out << "GetSubscriber returned " << subscriberPath << endl;
                }
                else {
                    out << "GetSubscriber error: invalid reply" << endl;
                    // Nullify the subscriber path so that we don't use it accidentally later
                    subscriberPath = "";
                }
            }
            else {
                out << "GetSubscriber error: wrong number of parameters" << endl;
            }
        } else if (QString("subscribe").startsWith(commandName)) {
            if (subscriberPath != "") {
                // We have a proper subscriber path
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
                    out << "Subscribe error: invalid reply";
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
                out << "Subscribe error: we don't have a subscriber" << endl;
            }
        } else if (QString("unsubscribe").startsWith(commandName)) {
            if (subscriberPath != "") {
                // We have a proper subscriber path
                QDBusInterface subscriber(busName, subscriberPath,
                                        "org.freedesktop.ContextKit.Subscriber", connection);
                // Call Subscribe synchronously
                subscriber.call("Unsubscribe", args);
            }
            else {
                out << "Unsubscribe error: we don't have a subscriber" << endl;
            }
        }
    }
}
