/* API definition */
#include "contextprovider.h"

/* ContextKit header files */
#include "Manager.h"

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h> // FIXME: Remove if not needed any more

struct Changeset{
	GHashTable* properties;
	GList* undeterminable_keys;
};

static ContextKitManager* manager = NULL;

int context_init(int keys_count, char** provided_keys, const char** dbus_service_name, SubscriberCallback first_subscriber, SubscriberCallback last_subscriber, GetCallback get) {

	g_type_init();

	// Initialize the needed ContextKit objects
	manager = context_kit_manager_get_instance();

	// TODO: Set the callbacks
	// TODO: Start providing services over dbus
	
	
	return 1;
}

ChangesetHandle context_changeset_create() {
	struct Changeset* set = malloc (sizeof (struct Changeset));
	
	set->properties = g_hash_table_new_full (g_str_hash, g_str_equal, &free, &free);
	set->undeterminable_keys = NULL;
	
	return (ChangesetHandle) set;
}

int context_changeset_add_int (ChangesetHandle h, const char *key, int value) {
	struct Changeset* set = (struct Changeset*) h;

	/* Create a GValue from the supplied data. */
	GValue* v = malloc(sizeof(GValue));
	memset (v, 0, sizeof (GValue));

	g_value_init(v, G_TYPE_INT);
	g_value_set_int(v, value);
	
	/* Store the key-value pair. */
	g_hash_table_insert(set->properties, g_strdup(key), v);
	// Note: Memory is allocated for the keys and the values.

	return 0;
}

int context_changeset_add_double (ChangesetHandle h, const char *key, double value) {
	struct Changeset* set = (struct Changeset*) h;

	/* Create a GValue from the supplied data. */
	GValue* v = malloc(sizeof(GValue));
	memset (v, 0, sizeof (GValue));

	g_value_init(v, G_TYPE_DOUBLE);
	g_value_set_int(v, value);
	
	/* Store the key-value pair. */
	g_hash_table_insert(set->properties, g_strdup(key), v);
	// Note: Memory is allocated for the keys and the values.

	return 0;
}

int context_changeset_add_string (ChangesetHandle h, const char *key, const char* value) {
	// TODO
	return 1;
}

int context_changeset_add_undetermined_key (ChangesetHandle h, const char *key) {
	struct Changeset* set = (struct Changeset*) h;
	set->undeterminable_keys = g_list_prepend (set->undeterminable_keys, strdup(key));
	// Note: Using prepend instead of append to avoid the inefficiency of adding multiple keys.
	// Note: Memory is allocated for the keys.
	return 0;
}

int context_changeset_send_notification(ChangesetHandle h) {
	struct Changeset* set = (struct Changeset*) h;
	/* Transfer the information to the manager. */
	if (manager == NULL) {// FIXME: needed?
		return 1;
	}
	context_kit_manager_property_values_changed(manager, set->properties, set->undeterminable_keys);
	return 0;
}

int context_changeset_delete(ChangesetHandle h) {
	struct Changeset* set = (struct Changeset*) h;

	/* Destroy the hash table */
	g_hash_table_destroy (set->properties);
	/* Note: The freeing function we supplied is used for freeing the keys and values.
	*/

	/* Destroy the keys in the list */
	g_list_foreach (set->undeterminable_keys, (GFunc) g_free, NULL);
	/* Free the list */
	g_list_free (set->undeterminable_keys);

	/* Free the allocated Changeset */
	free(set);
	return 0;
}

int context_no_of_subscribers(const char* key, int* p) {
	if (manager == NULL) { // FIXME: needed?
		return 1;
	}
	*p = context_kit_key_usage_counter_number_of_subscribers (context_kit_manager_get_key_counter (manager), key);
	return 0;
}

