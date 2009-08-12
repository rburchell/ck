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
#include <assert.h>

/* Keys (names of the context properties) divided in groups.
*/

const char *group1_keys[] = {
    "Group1.Int",
    "Group1.Double",
    "Group1.Bool",
    "Group1.String",
    NULL
};

const char *group2_keys[] = {
    "Group2.Key1",
    "Group2.Key2",
    NULL
};

const char *single1_key = "Single1.Key";
const char *single2_key = "Single2.Key";

/*
  Subscription statuses of the keys.
*/
int group1_subscribed = 0;
int group2_subscribed = 0;
int single1_subscribed = 0;
int single2_subscribed = 0;

/*
  This function is called when the subscription status of the
  group group1 changes.
*/
void group1_cb(int subscribed, void* user_data)
{
    group1_subscribed = subscribed;
}

/*
  This function is called when the subscription status of the
  group group2 changes.
*/
void group2_cb(int subscribed, void* user_data)
{
    group2_subscribed = subscribed;
}

/*
  This function is called when the subscription status of the
  key single1 changes.
*/
void single1_cb(int subscribed, void* user_data)
{
    single1_subscribed = subscribed;
}

/*
  This function is called when the subscription status of the
  key single2 changes.
*/
void single2_cb(int subscribed, void* user_data)
{
    single2_subscribed = subscribed;
}

/*
  Test cases. Note that the test cases are *not* independent and
  include only a minimal set of tests for the C api.
*/

int test_init()
{
    /* Test: initialize using the library */
    int ret = context_provider_init (DBUS_BUS_SESSION,
                                     "com.nokia.test");
    /* Expected result: return value == success */
    if (!ret) return 1;

    /* Install 2 groups of keys and 2 single keys */

    context_provider_install_group(group1_keys, TRUE,
                                   group1_cb, NULL);
    context_provider_install_group(group2_keys, FALSE,
                                   group2_cb, NULL);
    context_provider_install_key(single1_key, FALSE,
                                 single1_cb, NULL);
    context_provider_install_key(single2_key, FALSE,
                                 single2_cb, NULL);

    return 0;
}

int test_get_subscriber()
{
    return 0;
}

int test_subscription()
{
    return 0;
}

int test_value_changes()
{
    return 0;
}

int test_unsubscription()
{
    return 0;
}

int test_stopping()
{
    return 0;
}

int run_tests()
{
    int ret;

    fprintf(stderr, "Running test_init... ");
    ret = test_init();
    if (ret) {
        fprintf(stderr, "FAIL\n");
        return ret;
    }
    fprintf(stderr, "SUCCESS\n");

    /* All tests successful */
    return 0;
}

int main(int argc, char **argv)
{
    /* Initialization */
    GMainLoop *loop;
    g_type_init ();
    loop = g_main_loop_new (NULL, FALSE);

    /* Start the client program */
    FILE *client;
    client = popen("client 2>/dev/null", "w");
    assert(client);

    /* Test cases */
    int ret = run_tests();

    /* Command the client to exit */
    pclose(client);

    /* Return the success / failure value */
    return ret;
}
