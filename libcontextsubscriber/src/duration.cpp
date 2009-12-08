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

#include <QRegExp>
#include <QTime>
#include <QDate>
#include <QStringList>

#include "duration.h"

const qint64 Duration::NANOSECS_PER_MSEC;
const qint64 Duration::NANOSECS_PER_SEC;
const qint64 Duration::NANOSECS_PER_MIN;
const qint64 Duration::NANOSECS_PER_HOUR;
const qint64 Duration::NANOSECS_PER_DAY;
const qint64 Duration::NANOSECS_PER_WEEK;
const qint64 Duration::MSECS_PER_DAY;
const qint64 Duration::SECS_PER_DAY;
const qint64 Duration::DAYS_PER_YEAR;
const qint64 Duration::DAYS_PER_WEEK;

Duration::Duration(qint64 nanoSecs) : totalNanoSecs_p(nanoSecs)
{
    QTime reftime = QTime().addMSecs((nanoSecs/NANOSECS_PER_MSEC)% MSECS_PER_DAY);
    quint64 days = nanoSecs/NANOSECS_PER_DAY;

    nanoSecs_p = totalNanoSecs_p%(NANOSECS_PER_SEC);
    seconds_p = reftime.second();
    minutes_p = reftime.minute();
    hours_p = reftime.hour();
    days_p = (days%DAYS_PER_YEAR)%DAYS_PER_WEEK;
    weeks_p = (days%DAYS_PER_YEAR)/DAYS_PER_WEEK;
    years_p = days/DAYS_PER_YEAR;
}

Duration::Duration()
{
    totalNanoSecs_p = -1;
}

Duration::Duration(const Duration &val)
{
    totalNanoSecs_p = val.totalNanoSecs_p;
    nanoSecs_p = val.nanoSecs_p;
    seconds_p = val.seconds_p;
    minutes_p = val.minutes_p;
    hours_p = val.hours_p;
    days_p = val.days_p;
    weeks_p = val.weeks_p;
    years_p = val.years_p;
}

Duration::~Duration()
{
}

int Duration::nanoSecs() const
{
    return totalNanoSecs_p%(NANOSECS_PER_SEC);
}

int Duration::seconds() const
{
    return seconds_p;
}

int Duration::minutes() const
{
    return minutes_p;
}

int Duration::hours() const
{
    return hours_p;
}

int Duration::days() const
{
    return days_p;
}

int Duration::weeks() const
{
    return weeks_p;
}

int Duration::years() const
{
    return years_p;
}

QString Duration::toString() const
{
    QString retval;
    if(years_p) {
        retval.append(QString::number(years_p));
        retval.append(QString("Y "));
    }

    if(weeks_p) {
        retval.append(QString::number(weeks_p));
        retval.append(QString("W "));
    }

    if(days_p) {
        retval.append(QString::number(days_p));
        retval.append(QString("D "));
    }

    if(hours_p) {
        retval.append(QString::number(hours_p));
        retval.append(QString("H "));
    }

    if(minutes_p) {
        retval.append(QString::number(minutes_p));
        retval.append(QString("M "));
    }

    if(seconds_p) {
        retval.append(QString::number(seconds_p));
        retval.append(QString("S"));
    } else if (!years_p && !weeks_p && !days_p && !hours_p && !minutes_p && !seconds_p) {
        retval.append(QString("0S"));
    }
    return retval.simplified();
}

Duration Duration::fromString(const QString &duration)
{
    QRegExp re("(?:(\\d+)Y)?\\s*(?:(\\d+)W)?\\s*(?:(\\d+)D)?\\s*(?:(\\d+)H)?\\s*(?:(\\d+)M)?\\s*(?:(\\d+)S)?");
    re.setCaseSensitivity(Qt::CaseInsensitive);

    if (re.exactMatch(duration.simplified()) && re.matchedLength()){
        QStringList list;
        list = re.capturedTexts();
        list.removeFirst();

        qint64 nanosec_year = list.at(0).toLongLong()*(52*NANOSECS_PER_WEEK+NANOSECS_PER_DAY);
        qint64 nanosec_week = list.at(1).toLongLong()*NANOSECS_PER_WEEK;
        qint64 nanosec_day = list.at(2).toLongLong()*NANOSECS_PER_DAY;
        qint64 nanosec_hour = list.at(3).toLongLong()*NANOSECS_PER_HOUR;
        qint64 nanosec_min = list.at(4).toLongLong()*NANOSECS_PER_MIN;
        qint64 nanosec_sec = list.at(5).toLongLong()*NANOSECS_PER_SEC;
        return Duration(nanosec_year+nanosec_week+nanosec_day+nanosec_hour+nanosec_min+nanosec_sec);
    }
    else return Duration();
}

bool Duration::isNull() const
{
    return totalNanoSecs_p == -1;
}

bool Duration::isValid() const
{
    return !isNull();
}

qint64 Duration::toNanoSeconds() const
{
    return totalNanoSecs_p;
}

bool Duration::isDuration(const QString &duration)
{
    QRegExp re("(?:(\\d+)Y)?\\s*(?:(\\d+)W)?\\s*(?:(\\d+)D)?\\s*(?:(\\d+)H)?\\s*(?:(\\d+)M)?\\s*(?:(\\d+)S)?");
    re.setCaseSensitivity(Qt::CaseInsensitive);
    return re.exactMatch(duration.simplified()) && re.matchedLength();
}
