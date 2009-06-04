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
    
    static bool showTest; ///< Test messages enabled at runtime
    static bool showDebug; ///< Debug messages enabled at runtime
    static bool showWarning; ///< Warning messages enabled at runtime
    static bool showCritical; ///< Critical messages enabled at runtime
    static bool initialized; ///< Class initialized/env vars parsed
    static bool hideTimestamps; ///< Don't print timestamps
    static bool useColor; ///< Use simple colors for output (yellow for warnings, red for criticals)
    static char *showModule; ///< Show messages \b only from the specified module
    static char *hideModule; ///< Hide messages from the specified module
    
    static void initialize();
    
private:
    NullIODevice *nullDevice; ///< We use custom null-killer device when message disabled at runtime.
};

/*!
    \class ContextZeroLogger

    \brief A fake logging class.

    When a certain debug message is disabled at a compile-time the debug macros expand to 
    this class. It has all functions declared as \b inline and fundamentally kills all input
    targeted at it. The compiler optimizes the \b inline by not calling the functions at all and
    not storing the strings at all.
*/

class ContextZeroLogger
{
public:
    /// Constructor. Does nothing.
    inline ContextZeroLogger() {}
    
    /* Stubby ops */
   inline ContextZeroLogger &operator<< (QChar c) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (char c) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (signed short i) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (unsigned short i) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (signed int i) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (unsigned int i) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (signed long i) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (unsigned long i) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (float f) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (double f) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (const char * s) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (const QString &s) { return *this;} ///< Does nothing.
   inline ContextZeroLogger &operator<< (void * ptr) { return *this;} ///< Does nothing.
};

/* Macro defs */

#ifdef CONTEXT_LOG_HIDE_TEST
#define contextTest() (ContextZeroLogger())
#else
#define contextTest() (ContextRealLogger(CONTEXT_LOG_MSG_TYPE_TEST, __FUNCTION__, __FILE__, __LINE__))
#endif

#ifdef CONTEXT_LOG_HIDE_DEBUG
#define contextDebug() (ContextZeroLogger())
#else
#define contextDebug() (ContextRealLogger(CONTEXT_LOG_MSG_TYPE_DEBUG, __FUNCTION__, __FILE__, __LINE__))
#endif

#ifdef CONTEXT_LOG_HIDE_WARNING
#define contextWarning() (ContextZeroLogger())
#else
#define contextWarning() (ContextRealLogger(CONTEXT_LOG_MSG_TYPE_WARNING, __FUNCTION__, __FILE__, __LINE__))
#endif

#ifdef CONTEXT_LOG_HIDE_CRITICAL
#define contextCritical() (ContextZeroLogger())
#else
#define contextCritical() (ContextRealLogger(CONTEXT_LOG_MSG_TYPE_CRITICAL, __FUNCTION__, __FILE__, __LINE__))
#endif

#endif // LOGGING_H