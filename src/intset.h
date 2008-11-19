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

#ifndef __CONTEXT_KIT_INTSET_H__
#define __CONTEXT_KIT_INTSET_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * ContextKitIntSet:
 *
 * Opaque type representing a set of unsigned integers.
 */
typedef struct _ContextKitIntSet ContextKitIntSet;

/**
 * ContextKitIntFunc:
 * @i: The relevant integer
 * @userdata: Opaque user data
 *
 * A callback function acting on unsigned integers.
 */
typedef void (*ContextKitIntFunc) (guint i, gpointer userdata);

ContextKitIntSet *context_kit_intset_new (void);
ContextKitIntSet *context_kit_intset_sized_new (guint size);
void context_kit_intset_destroy (ContextKitIntSet *set);
void context_kit_intset_clear (ContextKitIntSet *set);

void context_kit_intset_add (ContextKitIntSet *set, guint element);
gboolean context_kit_intset_remove (ContextKitIntSet *set, guint element);
gboolean context_kit_intset_is_member (const ContextKitIntSet *set, guint element);

void context_kit_intset_foreach (const ContextKitIntSet *set, ContextKitIntFunc func,
    gpointer userdata);
GArray *context_kit_intset_to_array (const ContextKitIntSet *set);
ContextKitIntSet *context_kit_intset_from_array (const GArray *array);

guint context_kit_intset_size (const ContextKitIntSet *set);

gboolean context_kit_intset_is_equal (const ContextKitIntSet *left, const ContextKitIntSet *right);

ContextKitIntSet *context_kit_intset_copy (const ContextKitIntSet *orig);
ContextKitIntSet *context_kit_intset_intersection (const ContextKitIntSet *left, const ContextKitIntSet *right);
ContextKitIntSet *context_kit_intset_union (const ContextKitIntSet *left, const ContextKitIntSet *right);
ContextKitIntSet *context_kit_intset_difference (const ContextKitIntSet *left, const ContextKitIntSet *right);
ContextKitIntSet *context_kit_intset_symmetric_difference (const ContextKitIntSet *left,
    const ContextKitIntSet *right);

gchar *context_kit_intset_dump (const ContextKitIntSet *set);

typedef struct _ContextKitIntSetIter ContextKitIntSetIter;

/**
 * ContextKitIntSetIter:
 * @set: The set iterated over.
 * @element: Must be (guint)(-1) before iteration starts. Set to the next
 *  element in the set by context_kit_intset_iter_next(); undefined after
 *  context_kit_intset_iter_next() returns %FALSE.
 *
 * A structure representing iteration over a set of integers. Must be
 * initialized with either CONTEXT_KIT_INTSET_ITER_INIT() or context_kit_intset_iter_init().
 */
struct _ContextKitIntSetIter
{
    const ContextKitIntSet *set;
    guint element;
};

/**
 * CONTEXT_KIT_INTSET_ITER_INIT:
 * @set: A set of integers
 *
 * A suitable static initializer for a #ContextKitIntSetIter, to be used as follows:
 *
 * <informalexample><programlisting>
 * void
 * do_something (const ContextKitIntSet *intset)
 * {
 *   ContextKitIntSetIter iter = CONTEXT_KIT_INTSET_ITER_INIT (intset);
 *   /<!-- -->* ... do something with iter ... *<!-- -->/
 * }
 * </programlisting></informalexample>
 */
#define CONTEXT_KIT_INTSET_ITER_INIT(set) { (set), (guint)(-1) }

/**
 * context_kit_intset_iter_init:
 * @iter: An integer set iterator to be initialized.
 * @set: An integer set to be used by that iterator
 *
 * Reset the iterator @iter to the beginning and make it iterate over @set.
 */
static inline void
context_kit_intset_iter_init (ContextKitIntSetIter *iter, const ContextKitIntSet *set)
{
  g_return_if_fail (iter != NULL);
  iter->set = set;
  iter->element = (guint)(-1);
}

/**
 * context_kit_intset_iter_init:
 * @iter: An integer set iterator to be reset.
 *
 * Reset the iterator @iter to the beginning. It must already be associated
 * with a set.
 */
static inline void
context_kit_intset_iter_reset (ContextKitIntSetIter *iter)
{
  g_return_if_fail (iter != NULL);
  g_return_if_fail (iter->set != NULL);
  iter->element = (guint)(-1);
}

gboolean context_kit_intset_iter_next (ContextKitIntSetIter *iter);

G_END_DECLS

#endif /*__CONTEXT_KIT_INTSET_H__*/
