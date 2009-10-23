/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
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

#ifndef TIMEDVALUE_H
#define TIMEDVALUE_H

#include <time.h>
#include <QVariant>

namespace ContextSubscriber {

struct TimedValue
{
    quint64 time;
    QVariant value;

    TimedValue() : time(0), value(QVariant())
        { }
    TimedValue(const QVariant &value, quint64 time) : time(time), value(value)
        { }
    TimedValue(const QVariant &value) : value(value)
        {
            struct timespec t;
            clock_gettime(CLOCK_MONOTONIC, &t);
            time = t.tv_sec * Q_UINT64_C(1000000000) + t.tv_nsec;
        }
    bool operator<(const TimedValue &other)
		{
			return time < other.time;
		}
};

}

#endif
