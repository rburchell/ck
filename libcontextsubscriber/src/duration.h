/*
 * Copyright (C) 2008,2009 Nokia Corporation.
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

#ifndef DURATION_H
#define DURATION_H

#include <QString>
#include <QtCore/QMetaType>

class Duration
{

public:
    Duration();
    Duration(quint64 nanoSecs);
    Duration(const QString &duration);

    int nanoSecs() const;
    int seconds() const;
    int minutes() const;
    int hours() const;
    int days() const;
    int weeks() const;
    int years() const;
    QString toString() const;
    quint64 toNanoSeconds() const;
    bool operator==(const Duration &other) const;

    static bool isDuration(const QString &duration);
    static const qint64 NANOSECS_PER_MSEC = Q_INT64_C(1000000);
    static const qint64 NANOSECS_PER_SEC = 1000*NANOSECS_PER_MSEC;
    static const qint64 NANOSECS_PER_MIN = 60*NANOSECS_PER_SEC;
    static const qint64 NANOSECS_PER_HOUR = 60*NANOSECS_PER_MIN;
    static const qint64 NANOSECS_PER_DAY = 24*NANOSECS_PER_HOUR;
    static const qint64 NANOSECS_PER_WEEK = 7*NANOSECS_PER_DAY;
    static const qint64 SECS_PER_DAY = Q_INT64_C(86400);
    static const qint64 MSECS_PER_DAY = 1000*SECS_PER_DAY;
    static const qint64 DAYS_PER_YEAR = 365;
    static const qint64 DAYS_PER_WEEK = 7;

private:
    quint64 totalNanoSecs_p;///the duration expressed in nanoseconds
    int nanoSecs_p;///the number of nanoseconds in the duration
    int seconds_p;///the number of seconds in the duration
    int minutes_p;///the number of minutes in the duration
    int hours_p;///the number of hours in the duration
    int days_p;///the number of days in the duration
    int weeks_p;///the number of weeks in the duration
    int years_p;///the number of years in the duration

};
Q_DECLARE_METATYPE(Duration)
#endif
