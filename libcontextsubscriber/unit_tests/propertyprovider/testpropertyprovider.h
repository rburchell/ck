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

#include <QObject>
#include <QSet>
#include <QString>

class PropertyProvider;

class PropertyProviderUnitTests : public QObject
{
    Q_OBJECT

private:
    PropertyProvider *propertyProvider;

    // Tests
private slots:
    // Init and cleanup helper functions
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases
    void initializing();

    void getSubscriberSucceeds();
    void getSubscriberFails();

    void subscription();
    void unsubscription();

    void immediateUnsubscription();
    void immediateResubscription();

    void subscriptionFinished();

    void valuesChanged();

};

// Class for listening signals which have QSet<QString> as a parameter.
class CustomSignalListener : public QObject
{
    Q_OBJECT
public slots:
    void mySlot(QSet<QString>);
public:
    CustomSignalListener();
    int count;
    QList<QSet<QString> > parameters;
};
