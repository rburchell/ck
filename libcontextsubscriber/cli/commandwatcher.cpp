#include "commandwatcher.h"
#include "sconnect.h"
#include "contextproperty.h"
#include "propertylistener.h"
#include <QTextStream>
#include <QFile>
#include <QSocketNotifier>
#include <QStringList>
#include <QCoreApplication>
#include <QDebug>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <QMap>

CommandWatcher::CommandWatcher(int commandfd, QMap<QString, ContextProperty*> *properties, QObject *parent) :
    QObject(parent), properties(properties), commandfd(commandfd)
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
        qDebug() << "  new KEY...                      - create context properties";
        qDebug() << "  delete KEY...                   - delete context properties";
        qDebug() << "  subscribe KEY...                - subscribe to keys";
        qDebug() << "  waitforsubscription KEY...      - subscribe to keys";
        qDebug() << "  unsubscribe KEY...              - unsubscribe from keys";
        qDebug() << "  value KEY [DEF]                 - get value for a key";
        qDebug() << "Any prefix of a command can be used as an abbreviation";
}

void CommandWatcher::interpret(const QString& command) const
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

        if (QString("new").startsWith(commandName)) {
            foreach (QString key, args)
                if (properties->contains(key))
                    qDebug() << "key already exists: " << key;
                else {
                    properties->insert(key, new ContextProperty(key, QCoreApplication::instance()));
                    new PropertyListener(properties->value(key));
                }
        } else if (QString("delete").startsWith(commandName)) {
            foreach (QString key, args)
                if (properties->contains(key))
                    delete properties->take(key);
                else
                    qDebug() << "no such key:" << key;
        } else if (QString("subscribe").startsWith(commandName)) {
            foreach (QString key, args)
                if (properties->contains(key))
                    properties->value(key)->subscribe();
                else
                    qDebug() << "no such key:" << key;
        } else if (QString("waitforsubscription").startsWith(commandName)) {
            foreach (QString key, args)
                if (properties->contains(key)) {
                    properties->value(key)->waitForSubscription();
                    out << "wait finished for " << key << endl;
                } else
                    qDebug() << "no such key:" << key;
        } else if (QString("unsubscribe").startsWith(commandName)) {
            foreach (QString key, args)
                if (properties->contains(key))
                    properties->value(key)->unsubscribe();
                else
                    qDebug() << "no such key:" << key;
        } else if (QString("value").startsWith(commandName)) {
            QString key = args[0];
            if (properties->contains(key)) {
                QVariant value;
                if (args.size() > 1) {
                    value = properties->value(key)->value(args[1]);
                } else {
                    value = properties->value(key)->value();
                }
                if (value.isNull())
                    out << "value is Unknown" << endl;
                else
                    out << "value: " << value.typeName() << ":" << value.toString() << endl;
            } else
                qDebug() << "no such key:" << key;
        } else
            help();
    }
}
