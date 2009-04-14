/* intset.c - Source for a set of unsigned integers (implemented as a
 * variable length bitfield)
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

/**
 * SECTION:intset
 * @title: ContextProviderIntSet
 * @short_description: a set of unsigned integers
 * @see_also: #ContextProviderHandleSet
 *
 * A #ContextProviderIntSet is a set of unsigned integers, implemented as a
 * dynamically-allocated bitfield.
 */

#include "intset.h"

#include <string.h>
#include <glib.h>

#define DEFAULT_SIZE 16
#define DEFAULT_INCREMENT 8
#define DEFAULT_INCREMENT_LOG2 3

struct _ContextProviderIntSet
{
  guint32 *bits;
  guint size;
};

static ContextProviderIntSet *
_context_provider_intset_new_with_size (guint size)
{
  ContextProviderIntSet *set = g_slice_new (ContextProviderIntSet);
  set->size = MAX (size, DEFAULT_SIZE);
  set->bits = g_new0 (guint32, set->size);
  return set;
}

/**
 * context_provider_intset_sized_new:
 * @size: 1 more than the largest integer you expect to store
 *
 * Allocate an integer set just large enough to store the given number of bits,
 * rounded up as necessary.
 *
 * The set will still expand automatically if you store larger integers;
 * this is just an optimization to avoid wasting memory (if the set is too
 * large) or time (if the set is too small and needs reallocation).
 *
 * Returns: a new, empty integer set to be destroyed with context_provider_intset_destroy()
 */
ContextProviderIntSet *
context_provider_intset_sized_new (guint size)
{
  /* convert from a size in bits to a size in 32-bit words */
  if (G_UNLIKELY (size == 0))
    size = 1;
  else
    size = ((size - 1) >> 5) + 1;

  return _context_provider_intset_new_with_size (size);
}

/**
 * context_provider_intset_new:
 *
 * Allocate a new integer set with a default memory allocation.
 *
 * Returns: a new, empty integer set to be destroyed with context_provider_intset_destroy()
 */
ContextProviderIntSet *
context_provider_intset_new ()
{
  return _context_provider_intset_new_with_size (DEFAULT_SIZE);
}

/**
 * context_provider_intset_destroy:
 * @set: set
 *
 * Free all memory used by the set.
 */
void
context_provider_intset_destroy (ContextProviderIntSet *set)
{
  g_return_if_fail (set != NULL);

  g_free (set->bits);
  g_slice_free (ContextProviderIntSet, set);
}

/**
 * context_provider_intset_clear:
 * @set: set
 *
 * Unset every integer in the set.
 */
void
context_provider_intset_clear (ContextProviderIntSet *set)
{
  g_return_if_fail (set != NULL);

  memset (set->bits, 0, set->size * sizeof (guint32));
}

/**
 * context_provider_intset_add:
 * @set: set
 * @element: integer to add
 *
 * Add an integer into a ContextProviderIntSet.
 */
void
context_provider_intset_add (ContextProviderIntSet *set, guint element)
{
  guint offset;
  guint newsize;

  g_return_if_fail (set != NULL);

  offset = element >> 5;

  if (offset >= set->size)
  {
    newsize = ((offset>>DEFAULT_INCREMENT_LOG2) +1 ) << DEFAULT_INCREMENT_LOG2;
    set->bits = g_renew (guint32, set->bits, newsize);
    memset (set->bits + set->size, 0,
        sizeof (guint32) * (newsize - set->size));
    set->size = newsize;
    g_assert (offset<newsize);
  }
  set->bits[offset] = set->bits[offset] | (1<<(element & 0x1f));
}

/**
 * context_provider_intset_remove:
 * @set: set
 * @element: integer to add
 *
 * Remove an integer from a ContextProviderIntSet
 *
 * Returns: %TRUE if @element was previously in @set
 */
gboolean
context_provider_intset_remove (ContextProviderIntSet *set, guint element)
{
  guint offset;
  guint mask;

  g_return_val_if_fail (set != NULL, FALSE);

  offset = element >> 5;
  mask = 1 << (element & 0x1f);
  if (offset >= set->size)
    return FALSE;
  else if (!(set->bits[offset] & mask))
    return FALSE;
  else
    {
      set->bits[offset] &= ~mask;
      return TRUE;
    }
}

static inline gboolean
_context_provider_intset_is_member (const ContextProviderIntSet *set, guint element)
{
  guint offset;

  offset = element >> 5;
  if (offset >= set->size)
    return FALSE;
  else
    return (set->bits[offset] & (1 << (element & 0x1f))) != 0;
}


/**
 * context_provider_intset_is_member:
 * @set: set
 * @element: integer to test
 *
 * Tests if @element is a member of @set
 *
 * Returns: %TRUE if @element is in @set
 */
gboolean
context_provider_intset_is_member (const ContextProviderIntSet *set, guint element)
{
  g_return_val_if_fail (set != NULL, FALSE);

  return _context_provider_intset_is_member (set, element);
}

/**
 * context_provider_intset_is_subset_of:
 * @left: set
 * @right: set
 *
 * Tests if @left is a subset of @right
 *
 * Returns: %TRUE if @left is a subset of @right
 */
gboolean
context_provider_intset_is_subset_of (const ContextProviderIntSet *left, const ContextProviderIntSet *right)
{
  guint offset;
  gboolean ret = TRUE;

  if (left->size > right->size) {
    return FALSE;
  } else {
    for (offset = 0; offset < right->size; offset++) {
      ret = ret && (left->bits[offset] & right->bits[offset]) == left->bits[offset];

      if (!ret)
        break;
    }
  }
  return ret;
}

/**
 * context_provider_intset_is_disjoint:
 * @left: set
 * @right: set
 *
 * Tests if @right is disjoint with @left
 *
 * Returns: %TRUE if @right is disjoint with @left
 */
gboolean
context_provider_intset_is_disjoint (const ContextProviderIntSet *left, const ContextProviderIntSet *right)
{
  guint i;
  gboolean ret = TRUE;

  for (i = 0; i < MIN (right->size, left->size); i++) {
    ret = ret && (right->bits[i] & left->bits[i]) == 0;
  }

  return ret;
}


/**
 * context_provider_intset_foreach:
 * @set: set
 * @func: @ContextProviderIntFunc to use to iterate the set
 * @userdata: user data to pass to each call of @func
 *
 * Call @func(element, @userdata) for each element of @set.
 */

void
context_provider_intset_foreach (const ContextProviderIntSet *set, ContextProviderIntFunc func, gpointer userdata)
{
  guint i, j;

  g_return_if_fail (set != NULL);
  g_return_if_fail (func != NULL);

  for (i = 0; i < set->size; i++)
    {
      if (set->bits[i])
        for (j = 0; j < 32; j++)
          {
            if (set->bits[i] & 1 << j)
              func (i * 32 + j, userdata);
          }
    }
}

/**
 * context_provider_intset_size:
 * @set: A set of integers
 *
 * <!--Returns: says it all-->
 *
 * Returns: The number of integers in @set
 */

guint
context_provider_intset_size (const ContextProviderIntSet *set)
{
  guint i, count = 0;
  guint32 n;

  g_return_val_if_fail (set != NULL, 0);

  for (i = 0; i < set->size; i++)
    {
      n = set->bits[i];
      n = n - ((n >> 1) & 033333333333) - ((n >> 2) & 011111111111);
      count += ((n + (n >> 3)) & 030707070707) % 63;
    }

  return count;
}


/**
 * context_provider_intset_is_equal:
 * @left: A set of integers
 * @right: A set of integers
 *
 * <!--Returns: says it all-->
 *
 * Returns: %TRUE if @left and @right contain the same bits
 */

gboolean
context_provider_intset_is_equal (const ContextProviderIntSet *left, const ContextProviderIntSet *right)
{
  const ContextProviderIntSet *large, *small;
  guint i;

  g_return_val_if_fail (left != NULL, FALSE);
  g_return_val_if_fail (right != NULL, FALSE);

  if (left->size > right->size)
    {
      large = left;
      small = right;
    }
  else
    {
      large = right;
      small = left;
    }

  for (i = 0; i < small->size; i++)
    {
      if (large->bits[i] != small->bits[i])
        return FALSE;
    }

  for (i = small->size; i < large->size; i++)
    {
      if (large->bits[i] != 0)
        return FALSE;
    }

  return TRUE;
}


/**
 * context_provider_intset_copy:
 * @orig: A set of integers
 *
 * <!--Returns: says it all-->
 *
 * Returns: A set containing the same integers as @orig, to be freed with
 * context_provider_intset_destroy() by the caller
 */

ContextProviderIntSet *
context_provider_intset_copy (const ContextProviderIntSet *orig)
{
  ContextProviderIntSet *ret;

  g_return_val_if_fail (orig != NULL, NULL);

  ret = _context_provider_intset_new_with_size (orig->size);
  memcpy (ret->bits, orig->bits, (ret->size * sizeof (guint32)));

  return ret;
}


/**
 * context_provider_intset_intersection:
 * @left: The left operand
 * @right: The right operand
 *
 * <!--Returns: says it all-->
 *
 * Returns: The set of those integers which are in both @left and @right
 * (analogous to the bitwise operation left & right), to be freed with
 * context_provider_intset_destroy() by the caller
 */

ContextProviderIntSet *
context_provider_intset_intersection (const ContextProviderIntSet *left, const ContextProviderIntSet *right)
{
  const ContextProviderIntSet *large, *small;
  ContextProviderIntSet *ret;
  guint i;

  g_return_val_if_fail (left != NULL, NULL);
  g_return_val_if_fail (right != NULL, NULL);

  if (left->size > right->size)
    {
      large = left;
      small = right;
    }
  else
    {
      large = right;
      small = left;
    }

  ret = context_provider_intset_copy (small);

  for (i = 0; i < ret->size; i++)
    {
      ret->bits[i] &= large->bits[i];
    }

  return ret;
}


/**
 * context_provider_intset_union:
 * @left: The left operand
 * @right: The right operand
 *
 * <!--Returns: says it all-->
 *
 * Returns: The set of those integers which are in either @left or @right
 * (analogous to the bitwise operation left | right), to be freed with
 * context_provider_intset_destroy() by the caller
 */

ContextProviderIntSet *
context_provider_intset_union (const ContextProviderIntSet *left, const ContextProviderIntSet *right)
{
  const ContextProviderIntSet *large, *small;
  ContextProviderIntSet *ret;
  guint i;

  g_return_val_if_fail (left != NULL, NULL);
  g_return_val_if_fail (right != NULL, NULL);

  if (left->size > right->size)
    {
      large = left;
      small = right;
    }
  else
    {
      large = right;
      small = left;
    }

  ret = context_provider_intset_copy (large);

  for (i = 0; i < small->size; i++)
    {
      ret->bits[i] |= small->bits[i];
    }

  return ret;
}


/**
 * context_provider_intset_difference:
 * @left: The left operand
 * @right: The right operand
 *
 * <!--Returns: says it all-->
 *
 * Returns: The set of those integers which are in @left and not in @right
 * (analogous to the bitwise operation left & (~right)), to be freed with
 * context_provider_intset_destroy() by the caller
 */

ContextProviderIntSet *
context_provider_intset_difference (const ContextProviderIntSet *left, const ContextProviderIntSet *right)
{
  ContextProviderIntSet *ret;
  guint i;

  g_return_val_if_fail (left != NULL, NULL);
  g_return_val_if_fail (right != NULL, NULL);

  ret = context_provider_intset_copy (left);

  for (i = 0; i < MIN (right->size, left->size); i++)
    {
      ret->bits[i] &= ~right->bits[i];
    }

  return ret;
}


/**
 * context_provider_intset_symmetric_difference:
 * @left: The left operand
 * @right: The right operand
 *
 * <!--Returns: says it all-->
 *
 * Returns: The set of those integers which are in either @left or @right
 * but not both (analogous to the bitwise operation left ^ right), to be freed
 * with context_provider_intset_destroy() by the caller
 */

ContextProviderIntSet *
context_provider_intset_symmetric_difference (const ContextProviderIntSet *left, const ContextProviderIntSet *right)
{
  const ContextProviderIntSet *large, *small;
  ContextProviderIntSet *ret;
  guint i;

  g_return_val_if_fail (left != NULL, NULL);
  g_return_val_if_fail (right != NULL, NULL);

  if (left->size > right->size)
    {
      large = left;
      small = right;
    }
  else
    {
      large = right;
      small = left;
    }

  ret = context_provider_intset_copy (large);

  for (i = 0; i < small->size; i++)
    {
      ret->bits[i] ^= small->bits[i];
    }

  return ret;
}

static void
_dump_foreach (guint i, gpointer data)
{
   GString *tmp = (GString *) data;

  if (tmp->len == 0)
    g_string_append_printf (tmp, "%u", i);
  else
    g_string_append_printf (tmp, " %u", i);
}

/**
 * context_provider_intset_dump:
 * @set: An integer set
 *
 * <!--Returns: says it all-->
 *
 * Returns: a string which the caller must free with g_free, listing the
 * numbers in @set in a human-readable format
 */
gchar *
context_provider_intset_dump (const ContextProviderIntSet *set)
{
  GString *tmp = g_string_new ("");

  context_provider_intset_foreach (set, _dump_foreach, tmp);
  return g_string_free (tmp, FALSE);
}

/**
 * context_provider_intset_iter_next:
 * @iter: An iterator originally initialized with CONTEXT_PROVIDER_INTSET_INIT(set)
 *
 * If there are integers in (@iter->set) higher than (@iter->element), set
 * (iter->element) to the next one and return %TRUE. Otherwise return %FALSE.
 *
 * Usage:
 *
 * <informalexample><programlisting>
 * ContextProviderIntSetIter iter = CONTEXT_PROVIDER_INTSET_INIT (intset);
 * while (context_provider_intset_iter_next (&amp;iter))
 * {
 *   printf ("%u is in the intset\n", iter.element);
 * }
 * </programlisting></informalexample>
 *
 * Returns: %TRUE if (@iter->element) has been advanced
 */
gboolean
context_provider_intset_iter_next (ContextProviderIntSetIter *iter)
{
  g_return_val_if_fail (iter != NULL, FALSE);
  g_return_val_if_fail (iter->set != NULL, FALSE);

  do
    {
      if (iter->element == (guint)(-1))
        {
          /* only just started */
          iter->element = 0;
        }
      else
        {
          ++iter->element;
        }

      if (_context_provider_intset_is_member (iter->set, iter->element))
        {
          return TRUE;
        }
    }
  while (iter->element < (iter->set->size << 5)
         && iter->element != (guint)(-1));
  return FALSE;
}
