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

#include "sconnect.h"
#include "boolsysfspooler.h"
#include "logging.h"
#include "loggingfeatures.h"

BoolSysFsPooler::BoolSysFsPooler(const QString &fname) :
    input(fname), state(TriStateUnknown)
{
    contextDebug() << F_LOWMEM << "Opening" << fname << "in read mode.";

    if (! input.open(QIODevice::ReadOnly)) {
        contextWarning() << F_LOWMEM << "Failed to open" << fname << "for reading.";
        return;
    }
 
    watcher.addPath(fname);
    sconnect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged()));
    readState();
}

void BoolSysFsPooler::onFileChanged()
{
    contextDebug() << F_LOWMEM << "New data on" << input.fileName();
    readState();
}

void BoolSysFsPooler::readState()
{
    contextDebug() << F_LOWMEM << "Reading" << input.fileName() << "state";
    static char data[32];
    qint64 bytesRead = input.peek(data, 32);
    TriState newState;

    if (bytesRead > 0 && data[0] == '0')
        newState = TriStateTrue;
    else if (bytesRead > 0 && data[0] == '1')
        newState = TriStateFalse;
    else
        newState = TriStateUnknown;

    if (newState != state) {
        state = newState;
        emit stateChanged(newState);
    }
}

BoolSysFsPooler::TriState BoolSysFsPooler::getState()
{
    return state;
}

