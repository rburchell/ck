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
 
#ifndef LOGGING_H
#define LOGGING_H

#include <QtCore>

#define CONTEXT_LOG_MSG_TYPE_TEST       1
#define CONTEXT_LOG_MSG_TYPE_DEBUG      2
#define CONTEXT_LOG_MSG_TYPE_WARNING    3
#define CONTEXT_LOG_MSG_TYPE_CRITICAL   4

#ifndef CONTEXT_LOG_MODULE_NAME
#define CONTEXT_LOG_MODULE_NAME "unknown"
#endif

class NullIODevice : public QIODevice
{
public:
    virtual qint64 readData(char*, qint64);
    virtual qint64 writeData(const char*, qint64);
};

class ContextRealLogger : public QTextStream
{
public:
    ContextRealLogger(int msgType, const char *func, const char *file, int line);
    ~ContextRealLogger();
    void killOutput();
    
private:
    NullIODevice *nullDevice;
};

class ContextZeroLogger
{
public:
    inline ContextZeroLogger() {}    
    inline ContextZeroLogger& operator<< (const char *) { return *this;}
};

/* Macro defs */

#ifdef CONTEXT_LOG_DEBUG
#define contextDebug() (ContextRealLogger(CONTEXT_LOG_MSG_TYPE_DEBUG, __FUNCTION__, __FILE__, __LINE__))
#else
#define contextDebug() (ContextZeroLogger())
#endif

#ifdef CONTEXT_LOG_WARNING
#define contextWarning() (ContextRealLogger(CONTEXT_LOG_MSG_TYPE_WARNING, __FUNCTION__, __FILE__, __LINE__))
#else
#define contextWarning() (ContextZeroLogger())
#endif

#ifdef CONTEXT_LOG_CRITICAL
#define contextCritical() (ContextRealLogger(CONTEXT_LOG_MSG_TYPE_CRITICAL, __FUNCTION__, __FILE__, __LINE__))
#else
#define contextCritical() (ContextZeroLogger())
#endif

#ifdef CONTEXT_LOG_TEST
#define contextTest() (ContextRealLogger(CONTEXT_LOG_MSG_TYPE_TEST, __FUNCTION__, __FILE__, __LINE__))
#else
#define contextTest() (ContextZeroLogger())
#endif

#endif // LOGGING_H