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

#include "lowmemprovider.h"
#include "logging.h"
#include "loggingfeatures.h"
#include "sconnect.h"

#define LOW_WATERMARK   "/sys/kernel/low_watermark"
#define HIGH_WATERMARK  "/sys/kernel/high_watermark"

LowMemProvider::LowMemProvider()
{
}

QStringList LowMemProvider::keys()
{
    QStringList list;
    list << "System.MemoryPressure";
    return list;
}

void LowMemProvider::initialize()
{
    contextDebug() << F_LOWMEM << "Initializing lowmem plugin";
    
    memoryPressure = new Context("System.MemoryPressure", this);
    
    sconnect(memoryPressure, SIGNAL(onFirstSubscriberAppeared(QString)),
             this, SLOT(onFirstSubscriberAppeared()));
    sconnect(memoryPressure, SIGNAL(onLastSubscriberDisappeared(QString)),
             this, SLOT(onLastSubscriberDisappeared()));
}

void LowMemProvider::onFirstSubscriberAppeared()
{
    lowWM = new BoolSysFsPooler(LOW_WATERMARK);
    highWM = new BoolSysFsPooler(HIGH_WATERMARK);

    sconnect(lowWM, SIGNAL(onStateChanged(BoolSysFsPooler::TriState)), 
             this, SLOT(onWatermarkStateChanged()));
    sconnect(lowWM, SIGNAL(onStateChanged(BoolSysFsPooler::TriState)), 
             this, SLOT(onWatermarkStateChanged()));

    onWatermarkStateChanged();
}

void LowMemProvider::onLastSubscriberDisappeared()
{
    delete lowWM;
    lowWM = NULL;
    delete highWM;
    highWM = NULL;
}

void LowMemProvider::onWatermarkStateChanged()
{
    contextDebug() << F_LOWMEM << "Watermarks state change";

    BoolSysFsPooler::TriState lowWMState = lowWM->getState();
    BoolSysFsPooler::TriState highWMState = highWM->getState();

    if (lowWMState == BoolSysFsPooler::TriStateFalse && 
        highWMState == BoolSysFsPooler::TriStateFalse) {
        // normal
        memoryPressure->set(0);
    } else if (lowWMState == BoolSysFsPooler::TriStateTrue && 
               highWMState == BoolSysFsPooler::TriStateFalse) {
        // high
        memoryPressure->set(1);
    } else if (lowWMState == BoolSysFsPooler::TriStateTrue && 
               highWMState == BoolSysFsPooler::TriStateTrue) {
        // critical
        memoryPressure->set(2);
    } else if (lowWMState == BoolSysFsPooler::TriStateFalse && 
               highWMState == BoolSysFsPooler::TriStateTrue) {
        // ignore rogue state
        return;
    } else {
        // undefined */
        memoryPressure->unset();
        return;
    }
}
