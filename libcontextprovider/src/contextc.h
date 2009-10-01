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

#ifndef CONTEXT_C_H
#define CONTEXT_C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <dbus/dbus.h>

typedef void (*ContextProviderSubscriptionChangedCallback) (int subscribe, void* user_data);

int
context_provider_init           (DBusBusType bus_type, 
                                 const char* bus_name);

void        
context_provider_stop           (void);

void
context_provider_install_key    (const char* key, 
                                 int clear_values_on_subscribe, 
                                 ContextProviderSubscriptionChangedCallback subscription_changed_cb, 
                                 void* subscription_changed_cb_target);

void 
context_provider_install_group  (char** key_group, 
                                 int clear_values_on_subscribe, 
                                 ContextProviderSubscriptionChangedCallback subscription_changed_cb, 
                                 void* subscription_changed_cb_target);

void 
context_provider_set_integer    (const char* key, int value);

void 
context_provider_set_double     (const char* key, double value);

void 
context_provider_set_boolean    (const char* key, int value);

void
context_provider_set_string     (const char* key, const char* value);

void
context_provider_set_null       (const char* key);

#ifdef __cplusplus
}
#endif 

#endif
