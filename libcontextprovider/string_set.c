
#include <string_set.h>
#include <intset.h>
#include <gee/collection.h>
#include <gee/list.h>
#include <gobject/gvaluecollector.h>




struct _ContextProviderStringSetPrivate {
	ContextProviderIntSet* intset;
};

#define CONTEXT_PROVIDER_STRING_SET_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONTEXT_PROVIDER_TYPE_STRING_SET, ContextProviderStringSetPrivate))
enum  {
	CONTEXT_PROVIDER_STRING_SET_DUMMY_PROPERTY
};
static ContextProviderStringSet* context_provider_string_set_construct_from_intset (GType object_type, ContextProviderIntSet* intset);
static ContextProviderStringSet* context_provider_string_set_new_from_intset (ContextProviderIntSet* intset);
static void __lambda0 (guint i, void* data);
static void ___lambda0_context_provider_int_func (guint i, void* userdata);
struct _ContextProviderStringSetIterPrivate {
	ContextProviderStringSet* parent;
	ContextProviderIntSetIter iter;
};

#define CONTEXT_PROVIDER_STRING_SET_ITER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), CONTEXT_PROVIDER_STRING_SET_TYPE_ITER, ContextProviderStringSetIterPrivate))
enum  {
	CONTEXT_PROVIDER_STRING_SET_ITER_DUMMY_PROPERTY
};
static ContextProviderStringSetIter* context_provider_string_set_iter_construct (GType object_type, ContextProviderStringSet* parent);
static ContextProviderStringSetIter* context_provider_string_set_iter_new (ContextProviderStringSet* parent);
static gpointer context_provider_string_set_iter_parent_class = NULL;
static void context_provider_string_set_iter_finalize (ContextProviderStringSetIter* obj);
static gpointer context_provider_string_set_parent_class = NULL;
static void context_provider_string_set_finalize (ContextProviderStringSet* obj);
static gint _vala_array_length (gpointer array);



static ContextProviderStringSet* context_provider_string_set_construct_from_intset (GType object_type, ContextProviderIntSet* intset) {
	ContextProviderStringSet* self;
	ContextProviderIntSet* _tmp1;
	ContextProviderIntSet* _tmp0;
	g_return_val_if_fail (intset != NULL, NULL);
	self = (ContextProviderStringSet*) g_type_create_instance (object_type);
	_tmp1 = NULL;
	_tmp0 = NULL;
	self->priv->intset = (_tmp1 = (_tmp0 = intset, intset = NULL, _tmp0), (self->priv->intset == NULL) ? NULL : (self->priv->intset = (context_provider_intset_destroy (self->priv->intset), NULL)), _tmp1);
	(intset == NULL) ? NULL : (intset = (context_provider_intset_destroy (intset), NULL));
	return self;
}


static ContextProviderStringSet* context_provider_string_set_new_from_intset (ContextProviderIntSet* intset) {
	return context_provider_string_set_construct_from_intset (CONTEXT_PROVIDER_TYPE_STRING_SET, intset);
}


/**
 * context_provider_string_set_new:
 *
 * Create a new #ContextProviderStringSet
 *
 * Returns: a new #ContextProviderStringSet
 */
ContextProviderStringSet* context_provider_string_set_construct (GType object_type) {
	ContextProviderStringSet* self;
	ContextProviderIntSet* _tmp0;
	self = (ContextProviderStringSet*) g_type_create_instance (object_type);
	/* TODO:
	   we should have a way of statically initialising any keys that may be used in a stringset
	   so keys outside of this set are rejected
	   */
	_tmp0 = NULL;
	self->priv->intset = (_tmp0 = context_provider_intset_new (), (self->priv->intset == NULL) ? NULL : (self->priv->intset = (context_provider_intset_destroy (self->priv->intset), NULL)), _tmp0);
	return self;
}


ContextProviderStringSet* context_provider_string_set_new (void) {
	return context_provider_string_set_construct (CONTEXT_PROVIDER_TYPE_STRING_SET);
}


/**
 * context_provider_string_set_new_sized:
 *
 * Create a new #ContextProviderStringSet with an initially allocated size.
 *
 * Returns: a new #ContextProviderStringSet
 */
ContextProviderStringSet* context_provider_string_set_construct_sized (GType object_type, guint size) {
	ContextProviderStringSet* self;
	ContextProviderIntSet* _tmp0;
	self = (ContextProviderStringSet*) g_type_create_instance (object_type);
	_tmp0 = NULL;
	self->priv->intset = (_tmp0 = context_provider_intset_sized_new (size), (self->priv->intset == NULL) ? NULL : (self->priv->intset = (context_provider_intset_destroy (self->priv->intset), NULL)), _tmp0);
	return self;
}


ContextProviderStringSet* context_provider_string_set_new_sized (guint size) {
	return context_provider_string_set_construct_sized (CONTEXT_PROVIDER_TYPE_STRING_SET, size);
}


/**
 * context_provider_string_set_new_from_array:
 * @array: an exising array of strings, null terminated
 *
 * Create a new #ContextProviderStringSet from an existing string array.
 *
 * Returns: a new #ContextProviderStringSet
 */
ContextProviderStringSet* context_provider_string_set_construct_from_array (GType object_type, char** array) {
	ContextProviderStringSet* self;
	ContextProviderIntSet* _tmp0;
	self = (ContextProviderStringSet*) g_type_create_instance (object_type);
	_tmp0 = NULL;
	self->priv->intset = (_tmp0 = context_provider_intset_new (), (self->priv->intset == NULL) ? NULL : (self->priv->intset = (context_provider_intset_destroy (self->priv->intset), NULL)), _tmp0);
	{
		char** s_collection;
		int s_collection_length1;
		int s_it;
		s_collection = array;
		s_collection_length1 = _vala_array_length (array);
		for (s_it = 0; s_it < _vala_array_length (array); s_it = s_it + 1) {
			const char* _tmp1;
			char* s;
			_tmp1 = NULL;
			s = (_tmp1 = s_collection[s_it], (_tmp1 == NULL) ? NULL : g_strdup (_tmp1));
			{
				context_provider_string_set_add (self, s);
				s = (g_free (s), NULL);
			}
		}
	}
	return self;
}


ContextProviderStringSet* context_provider_string_set_new_from_array (char** array) {
	return context_provider_string_set_construct_from_array (CONTEXT_PROVIDER_TYPE_STRING_SET, array);
}


/**
 * context_provider_string_set_new_intersection:
 * @left: a #ContextProviderStringSet
 * @right: a #ContextProviderStringSet
 *
 * Create a new #ContextProviderStringSet by intersecting two existing string sets.
 *
 * Returns: a new #ContextProviderStringSet
 */
ContextProviderStringSet* context_provider_string_set_construct_intersection (GType object_type, ContextProviderStringSet* left, ContextProviderStringSet* right) {
	ContextProviderStringSet* self;
	g_return_val_if_fail (left != NULL, NULL);
	g_return_val_if_fail (right != NULL, NULL);
	self = (ContextProviderStringSet*) context_provider_string_set_construct_from_intset (object_type, context_provider_intset_intersection (left->priv->intset, right->priv->intset));
	return self;
}


ContextProviderStringSet* context_provider_string_set_new_intersection (ContextProviderStringSet* left, ContextProviderStringSet* right) {
	return context_provider_string_set_construct_intersection (CONTEXT_PROVIDER_TYPE_STRING_SET, left, right);
}


/**
 * context_provider_string_set_new_symmetric_difference:
 * @left: a #ContextProviderStringSet
 * @right: a #ContextProviderStringSet
 *
 * Create a new #ContextProviderStringSet by finding the symmetric difference
 * between two existing string sets.
 * Symmetric difference gives you the elements that are in either set, but not in both
 *
 * Returns: a new #ContextProviderStringSet
 */
ContextProviderStringSet* context_provider_string_set_construct_symmetric_difference (GType object_type, ContextProviderStringSet* left, ContextProviderStringSet* right) {
	ContextProviderStringSet* self;
	g_return_val_if_fail (left != NULL, NULL);
	g_return_val_if_fail (right != NULL, NULL);
	self = (ContextProviderStringSet*) context_provider_string_set_construct_from_intset (object_type, context_provider_intset_symmetric_difference (left->priv->intset, right->priv->intset));
	return self;
}


ContextProviderStringSet* context_provider_string_set_new_symmetric_difference (ContextProviderStringSet* left, ContextProviderStringSet* right) {
	return context_provider_string_set_construct_symmetric_difference (CONTEXT_PROVIDER_TYPE_STRING_SET, left, right);
}


/**
 * context_provider_string_set_new_difference:
 * @left: a #ContextProviderStringSet
 * @right: a #ContextProviderStringSet
 *
 * Create a new #ContextProviderStringSet by finding the difference
 * between two existing string sets.
 * Difference gives you the elements that are in #left, but not in #right.
 *
 * Returns: a new #ContextProviderStringSet
 */
ContextProviderStringSet* context_provider_string_set_construct_difference (GType object_type, ContextProviderStringSet* left, ContextProviderStringSet* right) {
	ContextProviderStringSet* self;
	g_return_val_if_fail (left != NULL, NULL);
	g_return_val_if_fail (right != NULL, NULL);
	self = (ContextProviderStringSet*) context_provider_string_set_construct_from_intset (object_type, context_provider_intset_difference (left->priv->intset, right->priv->intset));
	return self;
}


ContextProviderStringSet* context_provider_string_set_new_difference (ContextProviderStringSet* left, ContextProviderStringSet* right) {
	return context_provider_string_set_construct_difference (CONTEXT_PROVIDER_TYPE_STRING_SET, left, right);
}


/**
 * context_provider_string_set_new_union:
 * @left: a #ContextProviderStringSet
 * @right: a #ContextProviderStringSet
 *
 * Create a new #ContextProviderStringSet by finding the union
 * of two existing string sets.
 *
 * Returns: a new #ContextProviderStringSet
 */
ContextProviderStringSet* context_provider_string_set_construct_union (GType object_type, ContextProviderStringSet* left, ContextProviderStringSet* right) {
	ContextProviderStringSet* self;
	g_return_val_if_fail (left != NULL, NULL);
	g_return_val_if_fail (right != NULL, NULL);
	self = (ContextProviderStringSet*) context_provider_string_set_construct_from_intset (object_type, context_provider_intset_union (left->priv->intset, right->priv->intset));
	return self;
}


ContextProviderStringSet* context_provider_string_set_new_union (ContextProviderStringSet* left, ContextProviderStringSet* right) {
	return context_provider_string_set_construct_union (CONTEXT_PROVIDER_TYPE_STRING_SET, left, right);
}


/**
 * context_provider_string_set_add:
 * @self: a #ContextProviderStringSet
 * @element: a string
 *
 * Add a string to a stringset
 */
void context_provider_string_set_add (ContextProviderStringSet* self, const char* element) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (element != NULL);
	context_provider_intset_add (self->priv->intset, (guint) g_quark_from_string (element));
}


/**
 * context_provider_string_set_remove:
 * @self: a #ContextProviderStringSet
 * @element: a string
 *
 * Remove a string from a stringset
 *
 * Returns: TRUE if #element was a member of #self
 */
gboolean context_provider_string_set_remove (ContextProviderStringSet* self, const char* element) {
	GQuark q;
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (element != NULL, FALSE);
	q = g_quark_try_string (element);
	if (q != 0) {
		return context_provider_intset_remove (self->priv->intset, (guint) q);
	} else {
		return FALSE;
	}
}


/**
 * context_provider_string_set_size:
 * @self: a #ContextProviderStringSet
 *
 * Find the number of elements in a string set.
 *
 * Returns: the number of elements in #self.
 */
guint context_provider_string_set_size (ContextProviderStringSet* self) {
	g_return_val_if_fail (self != NULL, 0U);
	return context_provider_intset_size (self->priv->intset);
}


/**
 * context_provider_string_set_clear:
 * @self: a #ContextProviderStringSet
 *
 * Remove all elements from a string set.
 */
void context_provider_string_set_clear (ContextProviderStringSet* self) {
	g_return_if_fail (self != NULL);
	context_provider_intset_clear (self->priv->intset);
}


/**
 * context_provider_string_set_is_member:
 * @self: a #ContextProviderStringSet
 * @element: a string
 *
 * Find if #element is a member of the set.
 *
 * Returns: TRUE if #element is a member of #self
 */
gboolean context_provider_string_set_is_member (ContextProviderStringSet* self, const char* element) {
	GQuark q;
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (element != NULL, FALSE);
	q = g_quark_try_string (element);
	if (q != 0) {
		return context_provider_intset_is_member (self->priv->intset, (guint) q);
	} else {
		return FALSE;
	}
}


/**
 * context_provider_string_set_is_equal:
 * @self: a #ContextProviderStringSet
 * @string_set: a #ContextProviderStringSet
 *
 * Find if #string_set contains exactly the same elements as #self
 *
 * Returns: TRUE if #string_set is equivalent #self
 */
gboolean context_provider_string_set_is_equal (ContextProviderStringSet* self, ContextProviderStringSet* string_set) {
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (string_set != NULL, FALSE);
	return context_provider_intset_is_equal (self->priv->intset, string_set->priv->intset);
}


/**
 * context_provider_string_set_is_subset_of:
 * @self: a #ContextProviderStringSet
 * @string_set: a #ContextProviderStringSet
 *
 * Find if all the elements of #self are contained in #string_set
 *
 * Returns: TRUE if #string_set is a subset of #self
 */
gboolean context_provider_string_set_is_subset_of (ContextProviderStringSet* self, ContextProviderStringSet* string_set) {
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (string_set != NULL, FALSE);
	return context_provider_intset_is_subset_of (self->priv->intset, string_set->priv->intset);
}


/**
 * context_provider_string_set_is_disjoint:
 * @self: a #ContextProviderStringSet
 * @string_set: a #ContextProviderStringSet
 *
 * Find if #string_set contains exactly no elements in common with #self
 *
 * Returns: TRUE if #string_set is disjoint with #self
 */
gboolean context_provider_string_set_is_disjoint (ContextProviderStringSet* self, ContextProviderStringSet* string_set) {
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (string_set != NULL, FALSE);
	return context_provider_intset_is_disjoint (self->priv->intset, string_set->priv->intset);
}


/**
 * context_provider_string_set_iterator:
 * @self: a #ContextProviderStringSet
 *
 * Return a #ContextProviderStringSetIter onto @self.
 *
 * Returns: a ContextProviderStringSetIter
 */
ContextProviderStringSetIter* context_provider_string_set_iterator (ContextProviderStringSet* self) {
	ContextProviderStringSetIter* iter;
	g_return_val_if_fail (self != NULL, NULL);
	iter = context_provider_string_set_iter_new (self);
	return iter;
}


static void __lambda0 (guint i, void* data) {
	GeeArrayList* ret;
	ret = GEE_ARRAY_LIST (data);
	gee_collection_add ((GeeCollection*) ret, g_quark_to_string (((GQuark) i)));
}


static void ___lambda0_context_provider_int_func (guint i, void* userdata) {
	__lambda0 (i, userdata);
}


GeeArrayList* context_provider_string_set_to_array (ContextProviderStringSet* self) {
	ContextProviderIntFunc iter;
	GeeArrayList* ret;
	g_return_val_if_fail (self != NULL, NULL);
	iter = ___lambda0_context_provider_int_func;
	ret = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, g_direct_equal);
	context_provider_intset_foreach (self->priv->intset, iter, ret);
	return ret;
}


/**
 * context_provider_string_set_debug:
 * @self: a #ContextProviderStringSet
 *
 * Used to print the contents of a stringset.
 *
 * Returns: a string with the contents of "#self comma seperated.
 */
char* context_provider_string_set_debug (ContextProviderStringSet* self) {
	GeeArrayList* strings;
	char* ret;
	char* _tmp4;
	g_return_val_if_fail (self != NULL, NULL);
	strings = context_provider_string_set_to_array (self);
	if (gee_collection_get_size ((GeeCollection*) strings) < 1) {
		char* _tmp0;
		_tmp0 = NULL;
		return (_tmp0 = g_strdup (""), (strings == NULL) ? NULL : (strings = (g_object_unref (strings), NULL)), _tmp0);
	}
	ret = (char*) gee_list_get ((GeeList*) strings, 0);
	{
		gint i;
		i = 1;
		for (; i < gee_collection_get_size ((GeeCollection*) strings); i++) {
			char* _tmp3;
			char* _tmp2;
			char* _tmp1;
			_tmp3 = NULL;
			_tmp2 = NULL;
			_tmp1 = NULL;
			ret = (_tmp3 = g_strconcat (_tmp1 = g_strconcat (ret, ", ", NULL), _tmp2 = (char*) gee_list_get ((GeeList*) strings, i), NULL), ret = (g_free (ret), NULL), _tmp3);
			_tmp2 = (g_free (_tmp2), NULL);
			_tmp1 = (g_free (_tmp1), NULL);
		}
	}
	_tmp4 = NULL;
	return (_tmp4 = ret, (strings == NULL) ? NULL : (strings = (g_object_unref (strings), NULL)), _tmp4);
}


static ContextProviderStringSetIter* context_provider_string_set_iter_construct (GType object_type, ContextProviderStringSet* parent) {
	ContextProviderStringSetIter* self;
	ContextProviderStringSet* _tmp1;
	ContextProviderStringSet* _tmp0;
	ContextProviderIntSetIter _tmp2 = {0};
	g_return_val_if_fail (parent != NULL, NULL);
	self = (ContextProviderStringSetIter*) g_type_create_instance (object_type);
	_tmp1 = NULL;
	_tmp0 = NULL;
	self->priv->parent = (_tmp1 = (_tmp0 = parent, (_tmp0 == NULL) ? NULL : context_provider_string_set_ref (_tmp0)), (self->priv->parent == NULL) ? NULL : (self->priv->parent = (context_provider_string_set_unref (self->priv->parent), NULL)), _tmp1);
	self->priv->iter = (context_provider_intset_iter_init (&_tmp2, parent->priv->intset), _tmp2);
	return self;
}


static ContextProviderStringSetIter* context_provider_string_set_iter_new (ContextProviderStringSet* parent) {
	return context_provider_string_set_iter_construct (CONTEXT_PROVIDER_STRING_SET_TYPE_ITER, parent);
}


/**
 * context_provider_string_set_iter_next:
 * @self: a #ContextProviderStringSetIter
 *
 * Move the iterator onto the next element in the string set
 *
 * Returns: FALSE if there are no more elements in the set, TRUE otherwise.
 */
gboolean context_provider_string_set_iter_next (ContextProviderStringSetIter* self) {
	g_return_val_if_fail (self != NULL, FALSE);
	return context_provider_intset_iter_next (&self->priv->iter);
}


/**
 * context_provider_string_set_iter_get:
 * @self: a #ContextProviderStringSetIter
 *
 * Get the string currently pointed to by this iterator
 *
 * Returns: a string from the string set
 */
char* context_provider_string_set_iter_get (ContextProviderStringSetIter* self) {
	const char* _tmp0;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0 = NULL;
	return (_tmp0 = g_quark_to_string (((GQuark) self->priv->iter.element)), (_tmp0 == NULL) ? NULL : g_strdup (_tmp0));
}


static void context_provider_string_set_value_iter_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void context_provider_string_set_value_iter_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		context_provider_string_set_iter_unref (value->data[0].v_pointer);
	}
}


static void context_provider_string_set_value_iter_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = context_provider_string_set_iter_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer context_provider_string_set_value_iter_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* context_provider_string_set_value_iter_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		ContextProviderStringSetIter* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = context_provider_string_set_iter_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* context_provider_string_set_value_iter_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	ContextProviderStringSetIter** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags && G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = context_provider_string_set_iter_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* context_provider_string_set_param_spec_iter (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	ContextProviderStringSetParamSpecIter* spec;
	g_return_val_if_fail (g_type_is_a (object_type, CONTEXT_PROVIDER_STRING_SET_TYPE_ITER), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer context_provider_string_set_value_get_iter (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, CONTEXT_PROVIDER_STRING_SET_TYPE_ITER), NULL);
	return value->data[0].v_pointer;
}


void context_provider_string_set_value_set_iter (GValue* value, gpointer v_object) {
	ContextProviderStringSetIter* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, CONTEXT_PROVIDER_STRING_SET_TYPE_ITER));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, CONTEXT_PROVIDER_STRING_SET_TYPE_ITER));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		context_provider_string_set_iter_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		context_provider_string_set_iter_unref (old);
	}
}


static void context_provider_string_set_iter_class_init (ContextProviderStringSetIterClass * klass) {
	context_provider_string_set_iter_parent_class = g_type_class_peek_parent (klass);
	CONTEXT_PROVIDER_STRING_SET_ITER_CLASS (klass)->finalize = context_provider_string_set_iter_finalize;
	g_type_class_add_private (klass, sizeof (ContextProviderStringSetIterPrivate));
}


static void context_provider_string_set_iter_instance_init (ContextProviderStringSetIter * self) {
	self->priv = CONTEXT_PROVIDER_STRING_SET_ITER_GET_PRIVATE (self);
	self->ref_count = 1;
}


static void context_provider_string_set_iter_finalize (ContextProviderStringSetIter* obj) {
	ContextProviderStringSetIter * self;
	self = CONTEXT_PROVIDER_STRING_SET_ITER (obj);
	(self->priv->parent == NULL) ? NULL : (self->priv->parent = (context_provider_string_set_unref (self->priv->parent), NULL));
}


GType context_provider_string_set_iter_get_type (void) {
	static GType context_provider_string_set_iter_type_id = 0;
	if (context_provider_string_set_iter_type_id == 0) {
		static const GTypeValueTable g_define_type_value_table = { context_provider_string_set_value_iter_init, context_provider_string_set_value_iter_free_value, context_provider_string_set_value_iter_copy_value, context_provider_string_set_value_iter_peek_pointer, "p", context_provider_string_set_value_iter_collect_value, "p", context_provider_string_set_value_iter_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderStringSetIterClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) context_provider_string_set_iter_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ContextProviderStringSetIter), 0, (GInstanceInitFunc) context_provider_string_set_iter_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		context_provider_string_set_iter_type_id = g_type_register_fundamental (g_type_fundamental_next (), "ContextProviderStringSetIter", &g_define_type_info, &g_define_type_fundamental_info, 0);
	}
	return context_provider_string_set_iter_type_id;
}


gpointer context_provider_string_set_iter_ref (gpointer instance) {
	ContextProviderStringSetIter* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void context_provider_string_set_iter_unref (gpointer instance) {
	ContextProviderStringSetIter* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		CONTEXT_PROVIDER_STRING_SET_ITER_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}


static void context_provider_value_string_set_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void context_provider_value_string_set_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		context_provider_string_set_unref (value->data[0].v_pointer);
	}
}


static void context_provider_value_string_set_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = context_provider_string_set_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer context_provider_value_string_set_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* context_provider_value_string_set_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		ContextProviderStringSet* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = context_provider_string_set_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* context_provider_value_string_set_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	ContextProviderStringSet** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags && G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = context_provider_string_set_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* context_provider_param_spec_string_set (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	ContextProviderParamSpecStringSet* spec;
	g_return_val_if_fail (g_type_is_a (object_type, CONTEXT_PROVIDER_TYPE_STRING_SET), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer context_provider_value_get_string_set (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, CONTEXT_PROVIDER_TYPE_STRING_SET), NULL);
	return value->data[0].v_pointer;
}


void context_provider_value_set_string_set (GValue* value, gpointer v_object) {
	ContextProviderStringSet* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, CONTEXT_PROVIDER_TYPE_STRING_SET));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, CONTEXT_PROVIDER_TYPE_STRING_SET));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		context_provider_string_set_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		context_provider_string_set_unref (old);
	}
}


static void context_provider_string_set_class_init (ContextProviderStringSetClass * klass) {
	context_provider_string_set_parent_class = g_type_class_peek_parent (klass);
	CONTEXT_PROVIDER_STRING_SET_CLASS (klass)->finalize = context_provider_string_set_finalize;
	g_type_class_add_private (klass, sizeof (ContextProviderStringSetPrivate));
}


static void context_provider_string_set_instance_init (ContextProviderStringSet * self) {
	self->priv = CONTEXT_PROVIDER_STRING_SET_GET_PRIVATE (self);
	self->ref_count = 1;
}


static void context_provider_string_set_finalize (ContextProviderStringSet* obj) {
	ContextProviderStringSet * self;
	self = CONTEXT_PROVIDER_STRING_SET (obj);
	(self->priv->intset == NULL) ? NULL : (self->priv->intset = (context_provider_intset_destroy (self->priv->intset), NULL));
}


GType context_provider_string_set_get_type (void) {
	static GType context_provider_string_set_type_id = 0;
	if (context_provider_string_set_type_id == 0) {
		static const GTypeValueTable g_define_type_value_table = { context_provider_value_string_set_init, context_provider_value_string_set_free_value, context_provider_value_string_set_copy_value, context_provider_value_string_set_peek_pointer, "p", context_provider_value_string_set_collect_value, "p", context_provider_value_string_set_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (ContextProviderStringSetClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) context_provider_string_set_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ContextProviderStringSet), 0, (GInstanceInitFunc) context_provider_string_set_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		context_provider_string_set_type_id = g_type_register_fundamental (g_type_fundamental_next (), "ContextProviderStringSet", &g_define_type_info, &g_define_type_fundamental_info, 0);
	}
	return context_provider_string_set_type_id;
}


gpointer context_provider_string_set_ref (gpointer instance) {
	ContextProviderStringSet* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void context_provider_string_set_unref (gpointer instance) {
	ContextProviderStringSet* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		CONTEXT_PROVIDER_STRING_SET_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}


static gint _vala_array_length (gpointer array) {
	int length;
	length = 0;
	if (array) {
		while (((gpointer*) array)[length]) {
			length++;
		}
	}
	return length;
}




