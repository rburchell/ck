
#ifndef __KEY_USAGE_COUNTER_H__
#define __KEY_USAGE_COUNTER_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <string_set.h>

G_BEGIN_DECLS


#define CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER (context_provider_key_usage_counter_get_type ())
#define CONTEXT_PROVIDER_KEY_USAGE_COUNTER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER, ContextProviderKeyUsageCounter))
#define CONTEXT_PROVIDER_KEY_USAGE_COUNTER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER, ContextProviderKeyUsageCounterClass))
#define CONTEXT_PROVIDER_IS_KEY_USAGE_COUNTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER))
#define CONTEXT_PROVIDER_IS_KEY_USAGE_COUNTER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER))
#define CONTEXT_PROVIDER_KEY_USAGE_COUNTER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONTEXT_PROVIDER_TYPE_KEY_USAGE_COUNTER, ContextProviderKeyUsageCounterClass))

typedef struct _ContextProviderKeyUsageCounter ContextProviderKeyUsageCounter;
typedef struct _ContextProviderKeyUsageCounterClass ContextProviderKeyUsageCounterClass;
typedef struct _ContextProviderKeyUsageCounterPrivate ContextProviderKeyUsageCounterPrivate;
typedef struct _ContextProviderParamSpecKeyUsageCounter ContextProviderParamSpecKeyUsageCounter;

struct _ContextProviderKeyUsageCounter {
	GTypeInstance parent_instance;
	volatile int ref_count;
	ContextProviderKeyUsageCounterPrivate * priv;
};

struct _ContextProviderKeyUsageCounterClass {
	GTypeClass parent_class;
	void (*finalize) (ContextProviderKeyUsageCounter *self);
};

struct _ContextProviderParamSpecKeyUsageCounter {
	GParamSpec parent_instance;
};


ContextProviderKeyUsageCounter* context_provider_key_usage_counter_construct (GType object_type);
ContextProviderKeyUsageCounter* context_provider_key_usage_counter_new (void);
void context_provider_key_usage_counter_add (ContextProviderKeyUsageCounter* self, ContextProviderStringSet* keys);
void context_provider_key_usage_counter_remove (ContextProviderKeyUsageCounter* self, ContextProviderStringSet* keys);
gint context_provider_key_usage_counter_number_of_subscribers (ContextProviderKeyUsageCounter* self, const char* key);
ContextProviderStringSet* context_provider_key_usage_counter_get_subscribed_keys (ContextProviderKeyUsageCounter* self);
GParamSpec* context_provider_param_spec_key_usage_counter (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
gpointer context_provider_value_get_key_usage_counter (const GValue* value);
void context_provider_value_set_key_usage_counter (GValue* value, gpointer v_object);
GType context_provider_key_usage_counter_get_type (void);
gpointer context_provider_key_usage_counter_ref (gpointer instance);
void context_provider_key_usage_counter_unref (gpointer instance);


G_END_DECLS

#endif
