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

class PropertyHandle;

class PropertyHandleUnitTests : public QObject
{
    Q_OBJECT

private:
    PropertyHandle *propertyHandle;

    // Tests
private slots:
    // Init and cleanup helper functions
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases
    void initializing();
    void subscribe();
    void subscribeAndUnsubscribe();
    void subscribeTwice();
    void subscribeTwiceAndUnsubscribe();
    void subscribeTwiceAndUnsubscribeTwice();

    void setValueWithoutTypeCheck();
    void setValueWithTypeCheckAndCorrectTypes();
    void setValueWithTypeCheckAndIncorrectTypes();

    void commanderAppearsAndDisappears();

};
