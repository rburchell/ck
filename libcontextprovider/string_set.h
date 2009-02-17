
#ifndef __STRING_SET_H__
#define __STRING_SET_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <gee/arraylist.h>

G_BEGIN_DECLS


#define CONTEXT_PROVIDER_TYPE_STRING_SET (context_provider_string_set_get_type ())
#define CONTEXT_PROVIDER_STRING_SET(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_TYPE_STRING_SET, ContextProviderStringSet))
#define CONTEXT_PROVIDER_STRING_SET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), CONTEXT_PROVIDER_TYPE_STRING_SET, ContextProviderStringSetClass))
#define CONTEXT_PROVIDER_IS_STRING_SET(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_TYPE_STRING_SET))
#define CONTEXT_PROVIDER_IS_STRING_SET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONTEXT_PROVIDER_TYPE_STRING_SET))
#define CONTEXT_PROVIDER_STRING_SET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONTEXT_PROVIDER_TYPE_STRING_SET, ContextProviderStringSetClass))

typedef struct _ContextProviderStringSet ContextProviderStringSet;
typedef struct _ContextProviderStringSetClass ContextProviderStringSetClass;
typedef struct _ContextProviderStringSetPrivate ContextProviderStringSetPrivate;

#define CONTEXT_PROVIDER_STRING_SET_TYPE_ITER (context_provider_string_set_iter_get_type ())
#define CONTEXT_PROVIDER_STRING_SET_ITER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_STRING_SET_TYPE_ITER, ContextProviderStringSetIter))
#define CONTEXT_PROVIDER_STRING_SET_ITER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), CONTEXT_PROVIDER_STRING_SET_TYPE_ITER, ContextProviderStringSetIterClass))
#define CONTEXT_PROVIDER_STRING_SET_IS_ITER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_STRING_SET_TYPE_ITER))
#define CONTEXT_PROVIDER_STRING_SET_IS_ITER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONTEXT_PROVIDER_STRING_SET_TYPE_ITER))
#define CONTEXT_PROVIDER_STRING_SET_ITER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONTEXT_PROVIDER_STRING_SET_TYPE_ITER, ContextProviderStringSetIterClass))

typedef struct _ContextProviderStringSetIter ContextProviderStringSetIter;
typedef struct _ContextProviderStringSetIterClass ContextProviderStringSetIterClass;
typedef struct _ContextProviderStringSetIterPrivate ContextProviderStringSetIterPrivate;
typedef struct _ContextProviderStringSetParamSpecIter ContextProviderStringSetParamSpecIter;
typedef struct _ContextProviderParamSpecStringSet ContextProviderParamSpecStringSet;

/**
 * SECTION:string_set
 * @short_description: An unorder efficient set of strings
 * @include: string_set.h
 * @see_also: <link linkend="libcontextprovider-Context-provider-setup">Context Provider</link>
 *
 * A data structure which efficiently holds a set of strings. A string set has a maximum memory usage
 * of (total number of strings stored)/8 bytes.
 * All operations other than iteration are O(1)
 */
struct _ContextProviderStringSet {
	GTypeInstance parent_instance;
	volatile int ref_count;
	ContextProviderStringSetPrivate * priv;
};

struct _ContextProviderStringSetClass {
	GTypeClass parent_class;
	void (*finalize) (ContextProviderStringSet *self);
};

/**
 * ContextProviderStringSetIter:
 *
 * A structure used for iterating over a #ContextKitStringSet.
 */
struct _ContextProviderStringSetIter {
	GTypeInstance parent_instance;
	volatile int ref_count;
	ContextProviderStringSetIterPrivate * priv;
};

struct _ContextProviderStringSetIterClass {
	GTypeClass parent_class;
	void (*finalize) (ContextProviderStringSetIter *self);
};

struct _ContextProviderStringSetParamSpecIter {
	GParamSpec parent_instance;
};

struct _ContextProviderParamSpecStringSet {
	GParamSpec parent_instance;
};


ContextProviderStringSet* context_provider_string_set_construct (GType object_type);
ContextProviderStringSet* context_provider_string_set_new (void);
ContextProviderStringSet* context_provider_string_set_construct_sized (GType object_type, guint size);
ContextProviderStringSet* context_provider_string_set_new_sized (guint size);
ContextProviderStringSet* context_provider_string_set_construct_from_array (GType object_type, char** array);
ContextProviderStringSet* context_provider_string_set_new_from_array (char** array);
ContextProviderStringSet* context_provider_string_set_construct_intersection (GType object_type, ContextProviderStringSet* left, ContextProviderStringSet* right);
ContextProviderStringSet* context_provider_string_set_new_intersection (ContextProviderStringSet* left, ContextProviderStringSet* right);
ContextProviderStringSet* context_provider_string_set_construct_symmetric_difference (GType object_type, ContextProviderStringSet* left, ContextProviderStringSet* right);
ContextProviderStringSet* context_provider_string_set_new_symmetric_difference (ContextProviderStringSet* left, ContextProviderStringSet* right);
ContextProviderStringSet* context_provider_string_set_construct_difference (GType object_type, ContextProviderStringSet* left, ContextProviderStringSet* right);
ContextProviderStringSet* context_provider_string_set_new_difference (ContextProviderStringSet* left, ContextProviderStringSet* right);
ContextProviderStringSet* context_provider_string_set_construct_union (GType object_type, ContextProviderStringSet* left, ContextProviderStringSet* right);
ContextProviderStringSet* context_provider_string_set_new_union (ContextProviderStringSet* left, ContextProviderStringSet* right);
void context_provider_string_set_add (ContextProviderStringSet* self, const char* element);
gboolean context_provider_string_set_remove (ContextProviderStringSet* self, const char* element);
guint context_provider_string_set_size (ContextProviderStringSet* self);
void context_provider_string_set_clear (ContextProviderStringSet* self);
gboolean context_provider_string_set_is_member (ContextProviderStringSet* self, const char* element);
gboolean context_provider_string_set_is_equal (ContextProviderStringSet* self, ContextProviderStringSet* string_set);
gboolean context_provider_string_set_is_subset_of (ContextProviderStringSet* self, ContextProviderStringSet* string_set);
gboolean context_provider_string_set_is_disjoint (ContextProviderStringSet* self, ContextProviderStringSet* string_set);
ContextProviderStringSetIter* context_provider_string_set_iterator (ContextProviderStringSet* self);
GeeArrayList* context_provider_string_set_to_array (ContextProviderStringSet* self);
char* context_provider_string_set_debug (ContextProviderStringSet* self);
gboolean context_provider_string_set_iter_next (ContextProviderStringSetIter* self);
char* context_provider_string_set_iter_get (ContextProviderStringSetIter* self);
GParamSpec* context_provider_string_set_param_spec_iter (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
gpointer context_provider_string_set_value_get_iter (const GValue* value);
void context_provider_string_set_value_set_iter (GValue* value, gpointer v_object);
GType context_provider_string_set_iter_get_type (void);
gpointer context_provider_string_set_iter_ref (gpointer instance);
void context_provider_string_set_iter_unref (gpointer instance);
GParamSpec* context_provider_param_spec_string_set (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
gpointer context_provider_value_get_string_set (const GValue* value);
void context_provider_value_set_string_set (GValue* value, gpointer v_object);
GType context_provider_string_set_get_type (void);
gpointer context_provider_string_set_ref (gpointer instance);
void context_provider_string_set_unref (gpointer instance);


G_END_DECLS

#endif
