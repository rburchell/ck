/* tp-intset.h - Headers for a Glib-link set of integers
 *
 * Copyright (C) 2005, 2006 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2005, 2006 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
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

#ifndef __CONTEXT_PROVIDER_INTSET_H__
#define __CONTEXT_PROVIDER_INTSET_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * ContextProviderIntSet:
 *
 * Opaque type representing a set of unsigned integers.
 */
typedef struct _ContextProviderIntSet ContextProviderIntSet;

/**
 * ContextProviderIntFunc:
 * @i: The relevant integer
 * @userdata: Opaque user data
 *
 * A callback function acting on unsigned integers.
 */
typedef void (*ContextProviderIntFunc) (guint i, gpointer userdata);

ContextProviderIntSet *context_provider_intset_new (void);
ContextProviderIntSet *context_provider_intset_sized_new (guint size);
void context_provider_intset_destroy (ContextProviderIntSet *set);
void context_provider_intset_clear (ContextProviderIntSet *set);

void context_provider_intset_add (ContextProviderIntSet *set, guint element);
gboolean context_provider_intset_remove (ContextProviderIntSet *set, guint element);
gboolean context_provider_intset_is_member (const ContextProviderIntSet *set, guint element);

gboolean context_provider_intset_is_subset_of (const ContextProviderIntSet *left, const ContextProviderIntSet *right);
gboolean context_provider_intset_is_disjoint (const ContextProviderIntSet *left, const ContextProviderIntSet *right);

void context_provider_intset_foreach (const ContextProviderIntSet *set, ContextProviderIntFunc func,
    gpointer userdata);

guint context_provider_intset_size (const ContextProviderIntSet *set);

gboolean context_provider_intset_is_equal (const ContextProviderIntSet *left, const ContextProviderIntSet *right);

ContextProviderIntSet *context_provider_intset_copy (const ContextProviderIntSet *orig);
ContextProviderIntSet *context_provider_intset_intersection (const ContextProviderIntSet *left, const ContextProviderIntSet *right);
ContextProviderIntSet *context_provider_intset_union (const ContextProviderIntSet *left, const ContextProviderIntSet *right);
ContextProviderIntSet *context_provider_intset_difference (const ContextProviderIntSet *left, const ContextProviderIntSet *right);
ContextProviderIntSet *context_provider_intset_symmetric_difference (const ContextProviderIntSet *left,
    const ContextProviderIntSet *right);

gchar *context_provider_intset_dump (const ContextProviderIntSet *set);

typedef struct _ContextProviderIntSetIter ContextProviderIntSetIter;

/**
 * ContextProviderIntSetIter:
 * @set: The set iterated over.
 * @element: Must be (guint)(-1) before iteration starts. Set to the next
 *  element in the set by context_provider_intset_iter_next(); undefined after
 *  context_provider_intset_iter_next() returns %FALSE.
 *
 * A structure representing iteration over a set of integers. Must be
 * initialized with either CONTEXT_PROVIDER_INTSET_ITER_INIT() or context_provider_intset_iter_init().
 */
struct _ContextProviderIntSetIter
{
    const ContextProviderIntSet *set;
    guint element;
};

/**
 * CONTEXT_PROVIDER_INTSET_ITER_INIT:
 * @set: A set of integers
 *
 * A suitable static initializer for a #ContextProviderIntSetIter, to be used as follows:
 *
 * <informalexample><programlisting>
 * void
 * do_something (const ContextProviderIntSet *intset)
 * {
 *   ContextProviderIntSetIter iter = CONTEXT_PROVIDER_INTSET_ITER_INIT (intset);
 *   /<!-- -->* ... do something with iter ... *<!-- -->/
 * }
 * </programlisting></informalexample>
 */
#define CONTEXT_PROVIDER_INTSET_ITER_INIT(set) { (set), (guint)(-1) }

gboolean context_provider_intset_iter_next (ContextProviderIntSetIter *iter);

G_END_DECLS

#endif /*__CONTEXT_PROVIDER_INTSET_H__*/
