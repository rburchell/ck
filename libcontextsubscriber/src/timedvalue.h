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
    struct timespec time;
    QVariant value;

    TimedValue(const QVariant &value) : value(value)
        {
            clock_gettime(CLOCK_MONOTONIC, &time);
        }
    bool operator<(const TimedValue &other)
		{
			return ((time.tv_sec < other.time.tv_sec) ||
					(time.tv_sec == other.time.tv_sec &&
					 time.tv_nsec < other.time.tv_nsec));
		}
};

}

#endif
