typedef void* ChangesetHandle;

/* 
Callback function type for "first subscriber subscribed" and "last subscriber unsubscribed" notifications.
*/
typedef void (*SubscribeCallback)(int keys_count, const char** keys);

/* 
Callback function type for "Get is now executed" notifications.
*/
typedef void (*GetCallback)(int keys_count, const char** keys, ChangesetHandle h);

/*
Initialize the contextd.
*/
int context_init(int keys_count, char** provided_keys, const char** dbus_service_name, GetCallback get, SubscribeCallback first_subscriber, SubscribeCallback last_subscriber);

/*
Create a set of changed values
*/
ChangesetHandle context_changeset_create();

/*
Add data to the set of changed values.
*/
int context_changeset_add_int (ChangesetHandle h, const char *key, int value);
int context_changeset_add_double (ChangesetHandle h, const char *key, double value);
int context_changeset_add_string (ChangesetHandle h, const char *key, const char* value);

/*
Add a key to the list of undetermined keys.
*/
int context_changeset_add_undetermined_key (ChangesetHandle h, const char *key);

/*
Signal the change to subscribers.
*/
int context_changeset_send_notification(ChangesetHandle h);

/*
Release resources related to the set of changed values.
*/
int context_changeset_delete(ChangesetHandle h);

/*
Query the number of subscribers subscribed to a given key.
*/
int context_no_of_subscribers(const char* key, int* p);

/* FIXME: Documentation style? */


