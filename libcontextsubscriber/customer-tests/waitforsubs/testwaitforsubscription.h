/*
 * Copyright (C) 2008-2010 Nokia Corporation.
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

class QProcess;

class WaitForSubscriptionTests : public QObject
{
    Q_OBJECT

    public:
    WaitForSubscriptionTests();
public Q_SLOTS:
    void readStandardOutput();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void waitAndBlockExisting();
    void waitAndBlockNonExisting();

    void waitAndSpinExisting();
    void waitAndSpinNonExisting();

    void waitAndBlockSubscribed();
    void waitAndSpinSubscribed();

private:
    QProcess* provider;
    bool providerStarted;
    bool isReadyToRead;
};

class Helper : public QObject
{
    Q_OBJECT
public:
    bool processed;
    Helper() : processed(false)
    {
    }

public Q_SLOTS:
    void onTimeout()
    {
        processed = true;
    }
};
