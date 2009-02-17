
#ifndef __CONTEXT_PROVIDER_H__
#define __CONTEXT_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gee/arraylist.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <string_set.h>
#include <provider_interface.h>

G_BEGIN_DECLS


#define CONTEXT_PROVIDER_TYPE_CHANGE_SET (context_provider_change_set_get_type ())
#define CONTEXT_PROVIDER_CHANGE_SET(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_TYPE_CHANGE_SET, ContextProviderChangeSet))
#define CONTEXT_PROVIDER_CHANGE_SET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), CONTEXT_PROVIDER_TYPE_CHANGE_SET, ContextProviderChangeSetClass))
#define CONTEXT_PROVIDER_IS_CHANGE_SET(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_TYPE_CHANGE_SET))
#define CONTEXT_PROVIDER_IS_CHANGE_SET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONTEXT_PROVIDER_TYPE_CHANGE_SET))
#define CONTEXT_PROVIDER_CHANGE_SET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONTEXT_PROVIDER_TYPE_CHANGE_SET, ContextProviderChangeSetClass))

typedef struct _ContextProviderChangeSet ContextProviderChangeSet;
typedef struct _ContextProviderChangeSetClass ContextProviderChangeSetClass;
typedef struct _ContextProviderChangeSetPrivate ContextProviderChangeSetPrivate;
typedef struct _ContextProviderParamSpecChangeSet ContextProviderParamSpecChangeSet;
/**
 * ContextProviderGetCallback:
 * @keys: a #ContextProviderStringSet of keys for which values have been requested
 * @change_set: a #ContextProvideChangeSet in which to fill in the requested values
 * @user_data: the user data passed in #context_provider_install
 *
 * Type definition for a function that will be called back when key values are needed.
 * The keys will only ever be keys that this provider registered as providing.
 */
typedef void (*ContextProviderGetCallback) (ContextProviderStringSet* keys, ContextProviderChangeSet* change_set, void* user_data);
/**
 * ContextProviderSubscribedCallback:
 * @keys_subscribed: a #ContextProviderStringSet of keys which have been newly subscibed
 * @user_data: the user data passed in #context_provider_install
 *
 * Type definition for a function that will be called back when a key is first subscribed to
 * The keys will only ever be keys that this provider registered as providing.
 */
typedef void (*ContextProviderSubscribedCallback) (ContextProviderStringSet* keys_subscribed, void* user_data);
/**
 * ContextProviderUnsubscribedCallback:
 * @keys_unsubscribed: a #ContextProviderStringSet of keys that have been newly unsubscribed
 * @keys_remaining: a #ContextProviderStringSet of keys provided by this provider that are still
 * subscribed to
 * @user_data: the user data passed in #context_provider_install
 *
 * Type definition for a function that will be called back when a key is last unsubscribed from.
 * The keys will only ever be keys that this provider registered as providing.
 */
typedef void (*ContextProviderUnsubscribedCallback) (ContextProviderStringSet* keys_unsubscribed, ContextProviderStringSet* keys_remaining, void* user_data);

/**
 * SECTION:change_set
 * @short_description: Context value change handling
 * @include: context_provider.h
 * @see_also: #ContextProviderGetCallback
 *
 * #ContextProviderChangeSet is used to signal changes to contextual values by a provider.
 * As a provider you can asynchronously emit value changes for keys which you are providing
 * by creating a #ContextProviderChangeSet and commiting it using #context_provider_change_set_commit.
 * #ContextProviderChangeSet can also be provided to you, for example in
 * #ContextProviderGetCallback. In this case, you should simply add the values requested
 * and return without calling #context_provider_change_set_commit.
 */
struct _ContextProviderChangeSet {
	GTypeInstance parent_instance;
	volatile int ref_count;
	ContextProviderChangeSetPrivate * priv;
	GeeArrayList* undeterminable_keys;
};

struct _ContextProviderChangeSetClass {
	GTypeClass parent_class;
	void (*finalize) (ContextProviderChangeSet *self);
};

struct _ContextProviderParamSpecChangeSet {
	GParamSpec parent_instance;
};


extern gboolean context_provider_initialised;
ContextProviderChangeSet* context_provider_change_set_create (void);
gboolean context_provider_change_set_commit (ContextProviderChangeSet* change_set);
void context_provider_change_set_cancel (ContextProviderChangeSet* change_set);
ContextProviderChangeSet* context_provider_change_set_construct_from_get (GType object_type, GHashTable* properties, GeeArrayList* undeterminable_keys);
ContextProviderChangeSet* context_provider_change_set_new_from_get (GHashTable* properties, GeeArrayList* undeterminable_keys);
void context_provider_change_set_add_integer (ContextProviderChangeSet* self, const char* key, gint value);
void context_provider_change_set_add_double (ContextProviderChangeSet* self, const char* key, double value);
void context_provider_change_set_add_boolean (ContextProviderChangeSet* self, const char* key, gboolean value);
void context_provider_change_set_add_undetermined_key (ContextProviderChangeSet* self, const char* key);
GParamSpec* context_provider_param_spec_change_set (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
gpointer context_provider_value_get_change_set (const GValue* value);
void context_provider_value_set_change_set (GValue* value, gpointer v_object);
GType context_provider_change_set_get_type (void);
gpointer context_provider_change_set_ref (gpointer instance);
void context_provider_change_set_unref (gpointer instance);
gboolean context_provider_init (DBusBusType bus_type, const char* bus_name);
ContextProviderProvider* context_provider_install (char** provided_keys, ContextProviderGetCallback get_cb, void* get_cb_target, ContextProviderSubscribedCallback subscribed_cb, void* subscribed_cb_target, ContextProviderUnsubscribedCallback unsubscribed_cb, void* unsubscribed_cb_target);
void context_provider_remove (ContextProviderProvider* provider);
gint context_provider_no_of_subscribers (const char* key);


G_END_DECLS

#endif
