/*
 * Copyright (C) 2008 Nokia Corporation.
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

#ifndef CONTEXTPROPERTY_H
#define CONTEXTPROPERTY_H

#include <QObject>
#include <QVariant>
#include <QString>

class ContextPropertyPrivate;

class ContextProperty : public QObject
{
    Q_OBJECT

public:
    explicit ContextProperty(QObject *parent = 0);
    explicit ContextProperty(const QString &key, QObject *parent = 0);

    virtual ~ContextProperty();

    QString key() const;
    QVariant value(const QVariant &def) const;
    QVariant value() const;

    //ContextPropertyInfo info() const; //FIXME: enable when appropriate

    void subscribe () const;
    void unsubscribe () const;

    void waitForSubscription () const;

    ContextProperty(const ContextProperty &other, QObject *parent = 0);
    ContextProperty &operator=(const ContextProperty&);

    // This function should only be called by the Commander.
    // It sets all the ContextProperty instances immune to commanding.
    static void ignoreCommander();

    static void setTypeCheck(bool typeCheck);

signals:
    void valueChanged();

private:
    ContextPropertyPrivate *priv;

    void init (const QString &key);
    void finalize ();

    friend class ContextPropertyPrivate;
};

#endif // CONTEXTPROPERTY_H
