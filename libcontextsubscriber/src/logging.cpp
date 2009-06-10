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

#include "logging.h"
#include <stdio.h>

/*!
    \page Logging
    
    \brief The library (and ContexKit in general) use a simple logging system designed 
    to unify the output and make the debugging easier.
    
    \section API
    
    Four types of log messages (presented here in the order of importance) are supported: 
    \b Test, \b Debug, \b Warning and \b Critical. 
    
    The first one, the \b Test message requires some attention. It's meant to be used 
    from tests and unit-tests to log various stages of the test execution. It'll make
    the test output more easily filterable.
    
    The log messages can be used like this:
    
    \code
    contextTest() << "This is some message";
    contextDebug() << "My value is:" << someVariable;
    contextWarning() << "Expecting key:" << something.getKey();
    contextCritical() << 5 << " is bigger than " << 4;
    \endcode
    
    \section compilecontrol Compile-time verbosity control
    
    During the compile time certain defines can be used to turn-off debug messages. 
    Those defines are:
    
    \code
    CONTEXT_LOG_HIDE_TEST
    CONTEXT_LOG_HIDE_DEBUG
    CONTEXT_LOG_HIDE_WARNING
    CONTEXT_LOG_HIDE_CRITICAL
    \endcode
    
    A given define makes a respective macro message evaluate to an empty code. To be precise:
    it makes the macro message evaluate to an inline do-nothing class that is optimized by the
    compiler to do nothing.
    
    When ie. \c CONTEXT_LOG_HIDE_DEBUG define is used to turn off \c contextDebug()
    messages, the actual string content of the debug messages is \b not included in the binary 
    and during runtime the machine does not spend time evaluating it.
    
    Those compile-time control defines are integrated in the build/configure system.
    
    \section runtimecontrol Run-time verbosity control
    
    During run-time, the amount of debugging can be limited (filtered) but it can't be increased
    (expanded). In other words, if a package was compiled with warnings-only, it's not possible 
    to make it show debug messages at runtime. But it is possible to make it criticals-only. 
    
    The filtering happens via env variables. The major player is the \c CONTEXT_LOG_VERBOSITY variable
    which can be set to \c TEST, \c DEBUG, \c WARNING and \c CRITICAL. The \c CONTEXT_LOG_VERBOSITY 
    specifies the minimal level of the messages shown. Ie. \c CONTEXT_LOG_VERBOSITY set to 
    \c WARNING will show only warning and criticals. 
    
    The format of the output can be tweaked with \c CONTEXT_LOG_HIDE_TIMESTAMPS and \c CONTEXT_LOG_USE_COLOR. 
    The first one makes the messages shorter by skipping the timestamp info. The second one adds a 
    little bit of ANSI coloring to the messages.
    
    \c CONTEXT_LOG_SHOW_MODULE will filter-out (kill) all messages \b except the ones coming from the 
    specified module. Ie.:
    
    \code
    CONTEXT_LOG_SHOW_MODULE=subscriber ./some-binary
    \endcode
    
    ...will run \c ./some-binary showing log messages \b only from \c subscriber module.
    
    Lastly, \c CONTEXT_LOG_HIDE_MODULE will hide log messages coming from the specified module. 
    All other messages will be show.
*/

/* NullIODevice */

/*!
    \class NullIODevice  

    \brief Device that kills all input.

    This class is a \c QIODevice implementation that kills everything sent to it. 
    We set this as a output device for stream in ContextRealLogger when given debug
    message type was disabled at \b runtime. 
*/

qint64 NullIODevice::readData(char*, qint64)
{
    return 0;
}

qint64 NullIODevice::writeData(const char*, qint64)
{ 
    return 0; 
}

/* ContextRealLogger */

/*!
    \class ContextRealLogger

    \brief A real logging class.

    This is used by the actual macros to print messages.
*/

bool ContextRealLogger::showTest = true;
bool ContextRealLogger::showDebug = true;
bool ContextRealLogger::showWarning = true;
bool ContextRealLogger::showCritical = true;
bool ContextRealLogger::hideTimestamps = false;
bool ContextRealLogger::useColor = false;
char* ContextRealLogger::showModule = NULL;
char* ContextRealLogger::hideModule = NULL;
bool ContextRealLogger::initialized = false;

/// Initialize the class by checking the enviornment variables and setting 
/// the message output params. The log level is set from \c CONTEXT_LOG_VERBOSITY
/// and from this env var the showTest, showDebug, showWarning... are set. By default
/// everything is displayed at runtime. It's also possible to not show timestamps in 
/// messages and spice-up the output with some color.
void ContextRealLogger::initialize()
{
    if (getenv("CONTEXT_LOG_HIDE_TIMESTAMPS") != NULL)
        hideTimestamps = true;

    if (getenv("CONTEXT_LOG_USE_COLOR") != NULL)
        useColor = true;
        
    showModule = getenv("CONTEXT_LOG_SHOW_MODULE");
    hideModule = getenv("CONTEXT_LOG_HIDE_MODULE");

    const char *verbosity = getenv("CONTEXT_LOG_VERBOSITY");
    if (! verbosity)
        return;
                
    if (strcmp(verbosity, "TEST") == 0) {
        // Do nothing, all left true
    } else if (strcmp(verbosity, "DEBUG") == 0) {
        showTest = false;
    } else if (strcmp(verbosity, "WARNING") == 0) {
        showTest = false;
        showDebug = false;
    } else if (strcmp(verbosity, "CRITICAL") == 0) {
        showTest = false;
        showDebug = false;
        showWarning = false;
    }
    
    initialized = true;
}

/// Constructor. Called by the macros. \a func is the function name, \a file is
/// is the current source file and \a line specifies the line number.
ContextRealLogger::ContextRealLogger(int msgType, const char *func, const char *file, int line) 
    : QTextStream(stderr)
{
    nullDevice = NULL;
    
    if (! initialized) {
        // This is not thread safe, but our initialization depends on 
        // non-mutable parts anyways, so we should be ok.
        initialize();
    }
    
    const char *msgTypeString = NULL;
    
    // Killing by msg type
    if (msgType == CONTEXT_LOG_MSG_TYPE_DEBUG) {
        if (! showDebug) {
            killOutput();
            return;
        } else
            msgTypeString = "DEBUG";
    } else if (msgType == CONTEXT_LOG_MSG_TYPE_WARNING) {
        if (! showWarning) {
            killOutput();
            return;
        } else
            msgTypeString = (useColor) ? "\033[1;33mWARNING\033[0m" : "WARNING";
    } else if (msgType == CONTEXT_LOG_MSG_TYPE_CRITICAL) {
        if (! showCritical) {
            killOutput();
            return;
        } else
            msgTypeString = (useColor) ? "\033[1;31mCRITICAL\033[0m" : "CRITICAL";
    } else if (msgType == CONTEXT_LOG_MSG_TYPE_TEST) {
        if (! showTest) {
            killOutput();
            return;
        } else
            msgTypeString = "TEST";
    }
    
    // Killing if we're not the module we're interested in
    if (showModule && strcmp(showModule, CONTEXT_LOG_MODULE_NAME) != 0) {
        killOutput();
        return;
    }
    
    // Killing if we're the module we're NOT interested in
    if (hideModule && strcmp(hideModule, CONTEXT_LOG_MODULE_NAME) == 0) {
        killOutput();
        return;
    }
    
    if (! hideTimestamps)
        *this << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " ";
        
    *this << "[" << CONTEXT_LOG_MODULE_NAME << "]" << " ";
    *this << msgTypeString << " ";
    *this << "[" << file << ":" << line << ":" << func << "] ";
}

/// Make sure this logger will print nothing. 
void ContextRealLogger::killOutput()
{
    if (nullDevice)
        return;
            
    nullDevice = new NullIODevice();
    setDevice(new NullIODevice());
    return;
}

/// Prints \b end-of-line before going down.
ContextRealLogger::~ContextRealLogger()
{
    *this << "\n";
    delete nullDevice;
}
