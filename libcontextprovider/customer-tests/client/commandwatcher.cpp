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

#define PROPERTY "org.maemo.contextkit.Property"

CommandWatcher::CommandWatcher(int commandfd, QObject *parent) :
    QObject(parent), commandfd(commandfd), out(stdout), changedSignalReceived(false)
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
        qDebug() << "  assign BUSTYPE BUSNAME NAME     - assign BUSTYPE and BUSNAME to a custom NAME";
        qDebug() << "  get NAME KEY                    - get value of a key (long name) for a known NAME";
        qDebug() << "  subscribe NAME KEY              - subscribe to KEY for a known NAME";
        qDebug() << "  unsubscribe NAME KEY            - unsubscribe from KEY for a known NAME";
        qDebug() << "  resetsignalstatus               - forget any previously received ValueChanged signals";
        qDebug() << "  waitforchanged TIMEOUT          - wait until the ValueChanged signal arrives over DBus";
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
        if (QString("assign").startsWith(commandName)) {
            if (args.size() == 3) {
                // Create the DBus connection to the correct bus (session or system)
                QString busType = args.at(0);
                if (busType == "session" || busType == "system") {
                    // Store the bus type
                    QString busName = args.at(1);
                    QPair <QString, QString> pair(busType, busName);
                    connectionMap.insert(args.at(2), pair);
                    // Also start listening to ValueChanged signal
                    listenToChanged(args.at(2));
                    out << "Assigned " << args.at(2) << endl;
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
            if (args.size() == 2) {
                callSubscribe(args[0], args[1]);
            }
            else
                out << "Error: wrong number of parameters" << endl;
        } else if (QString("get").startsWith(commandName)) {
            if (args.size() == 2) {
                callGet(args[0], args[1]);
            }
            else
                out << "Error: wrong number of parameters" << endl;
        } else if (QString("unsubscribe").startsWith(commandName)) {
            if (args.size() == 2) {
                callUnsubscribe(args[0], args[1]);
            }
            else
                out << "Error: wrong number of parameters" << endl;
        } else if (QString("resetsignalstatus").startsWith(commandName)) {
            resetSignalStatus();
        } else if (QString("waitforchanged").startsWith(commandName)) {
            if (args.size() == 1) {
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
        else {
            out << "Error: invalid command" << endl;
        }
    }
}

void CommandWatcher::listenToChanged(const QString& name)
{
    if (connectionMap.contains(name) == false) {
        return;
    }
    QPair<QString, QString> connData = connectionMap[name];
    QDBusConnection connection = getConnection(connData.first);

    // Start listening to the Changed signal
    connection.connect(connData.second, "", PROPERTY, "ValueChanged",
                           this, SLOT(onValueChanged(QList<QVariant>,quint64,QDBusMessage)));

}

void CommandWatcher::callGet(const QString& busName, const QString& key)
{
    // Call Get synchronously
    if (connectionMap.contains(busName) == false) {
        out << "Error: Invalid name" << busName << endl;
        return;
    }
    QPair<QString, QString> connData = connectionMap[busName];
    QDBusConnection connection = getConnection(connData.first);

    QDBusPendingCall pc = connection.asyncCall(QDBusMessage::createMethodCall(connData.second,
                                                                              keyToPath(key),
                                                                              PROPERTY,
                                                                              "Get"));
    pc.waitForFinished();
    QDBusPendingReply<QList<QVariant>, quint64> reply = pc;
    if (reply.isError()) {
        out << "Get error: invalid reply" << endl;
        return;
    }
    out << "Get returned: " << describeValue(reply.argumentAt<0>(), reply.argumentAt<1>()) << endl;

}

void CommandWatcher::callSubscribe(const QString& name, const QString& key)
{
    // Call Subscribe synchronously
    if (connectionMap.contains(name) == false) {
        out << "Error: Invalid name" << name << endl;
        return;
    }
    QPair<QString, QString> connData = connectionMap[name];
    QDBusConnection connection = getConnection(connData.first);

    QDBusPendingCall pc = connection.asyncCall(QDBusMessage::createMethodCall(connData.second,
                                                                              keyToPath(key),
                                                                              PROPERTY,
                                                                              "Subscribe"));
    pc.waitForFinished();
    QDBusPendingReply<QList<QVariant>, quint64> reply = pc;
    if (reply.isError()) {
        out << "Subscribe error: invalid reply" << endl;
        return;
    }
    out << "Subscribe returned: " << describeValue(reply.argumentAt<0>(), reply.argumentAt<1>()) << endl;
}

void CommandWatcher::callUnsubscribe(const QString& name, const QString& key)
{
    // Call Unsubscribe synchronously
    if (connectionMap.contains(name) == false) {
        out << "Error: Invalid name" << name << endl;
        return;
    }
    QPair<QString, QString> connData = connectionMap[name];
    QDBusConnection connection = getConnection(connData.first);
    QDBusPendingCall pc = connection.asyncCall(QDBusMessage::createMethodCall(connData.second,
                                                                              keyToPath(key),
                                                                              PROPERTY,
                                                                              "Unsubscribe"));
    pc.waitForFinished();

    out << "Unsubscribe called" << endl;
}

void CommandWatcher::resetSignalStatus()
{
    changedSignalReceived = false;
    changedSignalParameters.clear();
    out << "Signal status resetted" << endl;
}

void CommandWatcher::waitForChanged(int timeout)
{
    QTime t;
    t.start();
    while (changedSignalReceived == false && t.elapsed() < timeout) {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    if (changedSignalReceived) {
        out << "Changed signal received, parameters: " << changedSignalParameters.at(0) << endl;
    }
    else {
        out << "Timeout" << endl;
    }
}

void CommandWatcher::onValueChanged(QList<QVariant> value, quint64 timestamp, QDBusMessage& msg)
{
    changedSignalReceived = true;
    changedSignalParameters.append(msg.path() + " " + describeValue(value, timestamp));
}

QString CommandWatcher::describeValue(QList<QVariant> value, quint64 timestamp)
{
    // Compose a string representation of the parameters
    if (value.size() == 0) {
        return "Unknown";
    }
    else {
        return describeQVariant(value[0]);
    }
    return "";
}

QString CommandWatcher::describeQVariant(QVariant value)
{
    QVariant::Type type = value.type();
    if (type == QVariant::Bool || type == QVariant::Int || type == QVariant::Double || type == QVariant::String) {
        // Automatic conversion is OK
        return value.toString();
    }
    if (type == QVariant::StringList) {
        return value.toStringList().join("/");
    }
    if (type == QVariant::UserType) {
        QDBusArgument dbusArgument = value.value<QDBusArgument >();
        return "";
        /* FIXME: How to extract types like QDate, QTime from this?
        If we know, what type to expect, it goes like this:

        QDate date;
        dbusArgument >> date;
        return date.toString();

        But how do we find out what type to expect?
        */
    }
    return "";
}

QDBusConnection CommandWatcher::getConnection(const QString& busType)
{
    if (busType == "system")
        return QDBusConnection::systemBus();
    return QDBusConnection::sessionBus();
}

QString CommandWatcher::keyToPath(QString key)
{
    if (key.startsWith("/"))
        return key;

    return "/org/maemo/contextkit/" + key.replace('.', '/');
}
