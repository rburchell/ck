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

#include <QRegExp>
#include <QTime>
#include <QDate>
#include <QStringList>

#include "duration.h"

/*!
  \class Duration

  \brief The Duration class represents a period of elapsed time between two events.

  A Duration object is created either by giving the number of nanoseconds explicitly or a string representation of a Duration.

  The Duration object assumes the following definitions:

  A year   = 52 weeks and 1 day OR 365 days

  A week   = 7 days

  A day    = 24 hours

  A hour   = 60 minutes

  A minute = 60 seconds

  A second = 10e9 nanoseconds

*/

///Convenient time conversions
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

/// Constructs a null Duration. A null Duration is invalid.
Duration::Duration()
{
    totalNanoSecs_p = 0;
}

/// Constructs a Duration with the given number of nanoseconds
Duration::Duration(quint64 nanoSecs) : totalNanoSecs_p(nanoSecs)
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

/// Constructs a copy of the other Duration.
Duration::Duration(const QString &duration)
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
        *this = Duration(nanosec_year+nanosec_week+nanosec_day+nanosec_hour+nanosec_min+nanosec_sec);
    } else {
        totalNanoSecs_p = 0;
    }
}
/// Returns true if this Duration is equal to the other Duration
bool Duration::operator==(const Duration &other) const {
    return (totalNanoSecs_p == other.totalNanoSecs_p);
}

/// Returns the number of nanoseconds in the duration
int Duration::nanoSecs() const
{
    return totalNanoSecs_p%(NANOSECS_PER_SEC);
}

/// Returns the number of seconds in the duration
int Duration::seconds() const
{
    return seconds_p;
}

/// Returns the number of minutes in the duration
int Duration::minutes() const
{
    return minutes_p;
}

/// Returns the number of hours in the duration
int Duration::hours() const
{
    return hours_p;
}

/// Returns the number of days in the duration
int Duration::days() const
{
    return days_p;
}

/// Returns the number of weeks in the duration
int Duration::weeks() const
{
    return weeks_p;
}

/// Returns the number of years in the duration
int Duration::years() const
{
    return years_p;
}
/// Returns a string representation of the duration
/// using the following format: Y W D H M S
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

/// Returns true if the Duration is null. A null Duration is invalid.
bool Duration::isNull() const
{
    return totalNanoSecs_p == 0;
}
/// Returns true if the Duration is not valid.
bool Duration::isValid() const
{
    return !isNull();
}

/// Returns the Duration expressed in nanoseconds
quint64 Duration::toNanoSeconds() const
{
    return totalNanoSecs_p;
}

/// Returns true if the string format used to represent the Duration is valid i.e. Y W D H M S , each being optional
bool Duration::isDuration(const QString &duration)
{
    QRegExp re("(?:(\\d+)Y)?\\s*(?:(\\d+)W)?\\s*(?:(\\d+)D)?\\s*(?:(\\d+)H)?\\s*(?:(\\d+)M)?\\s*(?:(\\d+)S)?");
    re.setCaseSensitivity(Qt::CaseInsensitive);
    return re.exactMatch(duration.simplified()) && re.matchedLength();
}
