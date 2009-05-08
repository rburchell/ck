/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -c SubscriberInterface -p subscriberiface_p.h:subscriberiface.cpp -i dbusvariantmap.h -N Subscriber.xml
 *
 * dbusxml2cpp is Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef SUBSCRIBERIFACE_P_H_1241787963
#define SUBSCRIBERIFACE_P_H_1241787963

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "dbusvariantmap.h"

/*
 * Proxy class for interface org.freedesktop.ContextKit.Subscriber
 */
class SubscriberInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.ContextKit.Subscriber"; }

public:
    SubscriberInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~SubscriberInterface();

public Q_SLOTS: // METHODS
    inline QDBusReply<DBusVariantMap> Subscribe(const QStringList &keys, QStringList &undeterminable_keys)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(keys);
        QDBusMessage reply = callWithArgumentList(QDBus::Block, QLatin1String("Subscribe"), argumentList);
        if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().count() == 2) {
            undeterminable_keys = qdbus_cast<QStringList>(reply.arguments().at(1));
        }
        return reply;
    }

    inline QDBusReply<void> Unsubscribe(const QStringList &keys)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(keys);
        return callWithArgumentList(QDBus::Block, QLatin1String("Unsubscribe"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void Changed(DBusVariantMap values, const QStringList &undeterminable_keys);
};

#endif