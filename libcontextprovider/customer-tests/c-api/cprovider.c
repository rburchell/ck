/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "context_provider.h"

#include <stdio.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <glib-object.h>

/* Keys (names of the context properties) divided in groups.
*/

const char *group1_keys[] = {
    "Group1.Key1",
    "Group1.Key2",
    NULL
};

const char *group2_keys[] = {
    "Group2.Key1",
    "Group2.Key2",
    NULL
};

const char *count_key = "Test.Count";

/* Values for the context properties.
 */
int poll_count = 0;
int throttled_poll_count = 0;

/* Publish the values of the context properties.
 */
void push_values()
{
    context_provider_set_integer ("Test.Count", throttled_poll_count);
}

/* Functions for updating the values.
 */
void poll_sensors()
{
    poll_count++;
    throttled_poll_count = poll_count - poll_count % 10;
}

guint timeout_id = 0;

gboolean timeout(gpointer data)
{
    poll_sensors();
    push_values();

    return TRUE;
}

void start_polling()
{
    if (timeout_id == 0) {
        g_message ("Start polling");

        /* Get initial values */
        poll_sensors();
        /* Publish the initial values */
        push_values();
        timeout_id = g_timeout_add (100, (GSourceFunc)timeout, NULL);
    }
}

void stop_polling()
{
    if (timeout_id) {
        g_message ("Stop polling");
        g_source_remove (timeout_id);
    }
    timeout_id = 0;
}

/*
  This function is called when the subscription status of the
  group group1 changes.
*/
void group1_cb(gboolean subscribed, void* user_data)
{
}

/*
  This function is called when the subscription status of the
  group group2 changes.
*/
void group2_cb(gboolean subscribed, void* user_data)
{
}

void key_cb(gboolean subscribed, void* user_data)
{
    if (subscribed) start_polling();
    else stop_polling();
}


int main(int argc, char **argv)
{
    GMainLoop *loop;

    g_type_init ();

    fprintf(stderr, "Calling context_provider_init\n");
    if (context_provider_init (DBUS_BUS_SESSION,
                               "com.nokia.test")) {
        fprintf(stderr, "Initialization successful\n");

        /* Install 3 groups of keys */

        context_provider_install_group(group1_keys, TRUE,
                                       group1_cb, NULL);
        context_provider_install_group(group2_keys, FALSE,
                                       group2_cb, NULL);

        /* Poll count (this group contains only one key) */
        context_provider_install_key(count_key, FALSE,
                                     key_cb, NULL);
        loop = g_main_loop_new (NULL, FALSE);
        g_main_loop_run (loop);
    }
    else {
        g_critical("Initializing the context provider library failed.");
        exit(1);
    }
    return 0;
}
