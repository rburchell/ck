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

struct ContextPtr;

ContextPtr* context_new             (const char *name);
int         context_init_service    (int is_system, const char *name, const char** keys, int key_count);
void        context_free            (ContextPtr *ptr);
int         context_is_valid        (ContextPtr *ptr);
int         context_is_set          (ContextPtr *ptr);
void        context_unset           (ContextPtr *ptr);
char*       context_get_key         (ContextPtr *ptr);
void        context_set_int         (ContextPtr *ptr, int v);
void        context_set_bool        (ContextPtr *ptr, int v);
void        context_set_string      (ContextPtr *ptr, const char *v);
void        context_set_double      (ContextPtr *ptr, double v);
int         context_get_int         (ContextPtr *ptr, int *v);
int         context_get_bool        (ContextPtr *ptr, int *v);
int         context_get_string      (ContextPtr *ptr, char **v);
int         context_get_double      (ContextPtr *ptr, double *v);

#ifdef __cplusplus
}
#endif 

#endif
