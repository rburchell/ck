/*
 * Copyright (C) 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "propertylistener.h"
#include "contextproperty.h"
#include "contextpropertyinfo.h"
#include "sconnect.h"
#include <QVariant>
#include <QCoreApplication>
#include <QDBusArgument>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <qjson/serializer.h>
#include <qjson/parser.h>
#else
#include <QJsonDocument>
#endif

PropertyListener::PropertyListener(const QString &key):
    QObject(QCoreApplication::instance()), prop(new ContextProperty(key, this))
{
    sconnect(prop, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
    if (!prop->info()->declared())
        QTextStream(stdout) << "Key doesn't exist: " + prop->key() << endl;
    else if (!prop->info()->provided())
        QTextStream(stdout) << "Key not provided: " + prop->key() << endl;
    QTextStream(stdout) << prop->key() << " subscribtion started" << endl;
}

QString PropertyListener::valueToString(QString defaultValue) const
{
    QVariant value;
    if (!defaultValue.isEmpty()) {
        bool isOk;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        QVariant def = QJson::Parser().parse(defaultValue.toUtf8(), &isOk);
#else
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(defaultValue.toUtf8(), &jsonError);
        isOk = jsonError.error == QJsonParseError::NoError;
        QVariant def = doc.toVariant();
#endif
        if (!isOk)
            qWarning() << "failed to parse default value as json:" << defaultValue;
        value = prop->value(def);
    } else {
        value = prop->value();
    }
    QString result;
    if (value.isNull())
        return result += "Unknown";
    result += value.typeName();
    result += ':';
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    result += QJson::Serializer().serialize(value);
#else
    QJsonDocument doc = QJsonDocument::fromVariant(value);
    result += doc.toJson();
#endif
    return result;
}

void PropertyListener::onValueChanged()
{
    QTextStream out(stdout);
    out << prop->key() << " = " << valueToString() << endl;
}
