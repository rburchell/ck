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
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

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

int group1_cb_callcount = 0;
int group2_cb_callcount = 0;
int single1_cb_callcount = 0;
int single2_cb_callcount = 0;

/*
  This function is called when the subscription status of the
  group group1 changes.
*/
void group1_cb(int subscribed, void* user_data)
{
    group1_subscribed = subscribed;
    ++ group1_cb_callcount;
}

/*
  This function is called when the subscription status of the
  group group2 changes.
*/
void group2_cb(int subscribed, void* user_data)
{
    group2_subscribed = subscribed;
    ++ group2_cb_callcount;
}

/*
  This function is called when the subscription status of the
  key single1 changes.
*/
void single1_cb(int subscribed, void* user_data)
{
    single1_subscribed = subscribed;
    ++ single1_cb_callcount;
}

/*
  This function is called when the subscription status of the
  key single2 changes.
*/
void single2_cb(int subscribed, void* user_data)
{
    single2_subscribed = subscribed;
    ++ single2_cb_callcount;
}

/* Pipes to and from the client program.  The test program commands
   the client and checks the output through them.
 */
int tube[2];
char line_from_client[1024];
int chars_read = 0;

char* write_to_client(char* command)
{
    fd_set read_fds;
    struct timeval tv;
    int ret = -1;

    /* Write the command to the client */
    dprintf(tube[1], command);

    /* Process the events until we can read from the client */
    do {
        /*fprintf(stderr, "processing\n");*/
        g_main_context_iteration(NULL, FALSE);

        FD_ZERO(&read_fds);
        FD_SET(tube[1], &read_fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        /* Note: the "number of file descriptors to check" is not
           1. It means "starting from file descriptor 0, how many file
           descriptors I need to check".
        */
        ret = select(tube[1]+1, &read_fds, NULL, NULL, &tv);
    } while (ret == 0 || ret == -1);

    chars_read = read(tube[1], line_from_client, 1023);
    line_from_client[chars_read] = '\0';
    /*fprintf(stderr, "\"%s\"\n", line_from_client);*/

    return 0;
}

/* A helper function for comparing the actual output (from the client)
   to expected output.
 */
int compare_output(char* expected)
{
    int i = 0;
    if (strncmp(line_from_client, expected, chars_read - 1)) {
        fprintf(stderr, "\nActual output  : \"%s\"\n", line_from_client);
        fprintf(stderr, "Expected output: \"%s\"\n", expected);
        fprintf(stderr, "Characters read: \"");
        for (i=0; i < chars_read; ++i) fprintf(stderr, "*");
        fprintf(stderr, "\n");
        return 1;
    }
    return 0;
}

/*
  Test cases. Note that the test cases are *not* independent and
  include only a minimal set of tests for the C api.
*/
int test_init()
{
    /* Test: initialize using the library */
    int ret = context_provider_init (DBUS_BUS_SESSION,
                                     "org.freedesktop.ContextKit.testProvider");
    /* Expected result: return value == success */
    if (!ret) return 1;

    /* Install 2 groups of keys and 2 single keys */

    context_provider_install_group(group1_keys, TRUE,
                                   group1_cb, NULL);
    context_provider_install_group(group2_keys, FALSE,
                                   group2_cb, NULL);
    context_provider_install_key(single1_key, TRUE,
                                 single1_cb, NULL);
    context_provider_install_key(single2_key, FALSE,
                                 single2_cb, NULL);

    /* Process the events, hoping that the DBus objects now get
     * initialized properly */
    while (g_main_context_pending(NULL)) {
        g_main_context_iteration(NULL, FALSE);
    }
    return 0;
}

int test_get_subscriber()
{
    /* Test: Command the client to execute GetSubscriber over DBus. */
    write_to_client("getsubscriber "
                    "session org.freedesktop.ContextKit.testProvider\n");

    /* Expected result: the client got a correct subscriber path */
    int mismatch = compare_output("GetSubscriber returned "
                           "/org/freedesktop/ContextKit/Subscriber/0\n");
    if (mismatch) return 1;
    return 0;
}

int test_subscription()
{
    /* Test: Command the client to execute Subscribe over DBus. */
    write_to_client("subscribe org.freedesktop.ContextKit.testProvider "
                    "Group1.Int\n");

    /* Expected result: the client got the key as Unknown */
    int mismatch = compare_output("Known keys: Unknown keys: Group1.Int \n");
    if (mismatch) return 1;

    /* Expected result: we are notified that the client is now subscribed */
    if (!group1_subscribed) return 1;
    if (group1_cb_callcount != 1) return 1;

    /* Test: set a value to a key and command the client to subscribe to it */
    context_provider_set_double("Group1.Double", 55.2);
    write_to_client("subscribe org.freedesktop.ContextKit.testProvider "
                    "Group1.Double\n");

    /* Expected result: the client got the key and the value */
    mismatch = compare_output("Known keys: Group1.Double(double:55.2) "
                                  "Unknown keys:  \n");
    if (mismatch) return 1;

    /* Expected result: we are still subscribed but not notified again */
    if (!group1_subscribed) return 1;
    if (group1_cb_callcount != 1) return 1;

    /* Test: set 2 more values and command the client to subscribe to them */
    context_provider_set_string("Group1.String", "teststring");
    context_provider_set_boolean("Group1.Bool", 1);

    write_to_client("subscribe org.freedesktop.ContextKit.testProvider "
                    "Group1.String Group1.Bool Invalid.Key\n");

    /* Expected result: the client got the keys and the values */
    mismatch = compare_output("Known keys: Group1.Bool(bool:true) "
                              "Group1.String(QString:teststring) "
                              "Unknown keys:  \n");
    if (mismatch) return 1;

    /* Expected result: we are still subscribed but not notified again */
    if (!group1_subscribed) return 1;
    if (group1_cb_callcount != 1) return 1;

    /* Expected result: during the whole test, non-relevant callbacks
     * are not called */
    if (group2_cb_callcount != 0) return 1;
    if (single1_cb_callcount != 0) return 1;
    if (single2_cb_callcount != 0) return 1;

    return 0;
}

int test_callbacks()
{
    /* Test: Command the client to subscribe to a property */
    write_to_client("subscribe org.freedesktop.ContextKit.testProvider "
                    "Single1.Key\n");

    /* Expected result: we are notified that the client is now subscribed */
    if (!single1_subscribed) return 1;
    if (single1_cb_callcount != 1) return 1;

    /* Test: Command the client to unsubscribe */
    write_to_client("unsubscribe org.freedesktop.ContextKit.testProvider "
                    "Single1.Key\n");

    /* Expected result: we are notified that the client is now unsubscribed */
    if (single1_subscribed) return 1;
    if (single1_cb_callcount != 2) return 1;

    return 0;
}

int test_value_changes()
{
    /* Test: change a subscribed property and command the client to
     * wait the Changed signal */
    context_provider_set_double("Group1.Double", -41.987);
    write_to_client("waitforchanged 3000\n");

    /* Expected result: the client got the signal */
    int mismatch = compare_output("Changed signal received, parameters: "
                              "Known keys: Group1.Double(double:-41.987) "
                              "Unknown keys:  \n");
    if (mismatch) return 1;

    write_to_client("resetsignalstatus\n");

    /* Test: value changes to unknown */
    context_provider_set_null("Group1.String");
    write_to_client("waitforchanged 3000\n");

    /* Expected result: the client got the signal */
    mismatch = compare_output("Changed signal received, parameters: "
                              "Known keys: "
                              "Unknown keys: Group1.String \n");
    if (mismatch) return 1;

    write_to_client("resetsignalstatus\n");

    /* Test: non-subscribed property changes */
    context_provider_set_integer("Group2.Key1", -365);
    write_to_client("waitforchanged 3000\n");

    /* Expected result: the client doesn't get a signal */
    mismatch = compare_output("Timeout\n");
    if (mismatch) return 1;

    /* Test: two properties change at the same time */
    context_provider_set_integer("Group1.Int", 343);
    context_provider_set_boolean("Group1.Bool", 0);

    write_to_client("waitforchanged 3000\n");

    /* Expected result: the client gets both in the same signal */
    mismatch = compare_output("Changed signal received, parameters: "
                              "Known keys: Group1.Bool(bool:false) "
                              "Group1.Int(int:343) "
                              "Unknown keys: \n");
    if (mismatch) return 1;

    write_to_client("resetsignalstatus\n");

    /* Test: set two properties the same values they already have */
    context_provider_set_integer("Group1.Int", 343);
    context_provider_set_boolean("Group1.Bool", 0);

    write_to_client("waitforchanged 3000\n");

    /* Expected result: the client doesn't get a signal */
    mismatch = compare_output("Timeout\n");
    if (mismatch) return 1;

    return 0;
}

int test_unsubscription()
{
    /* Test: Command the client to unsubscribe from all Group1 keys. */
    write_to_client("unsubscribe org.freedesktop.ContextKit.testProvider "
                    "Group1.Int Group1.Bool Group1.Double Group1.String\n");
    /* Expected result: we are notified that group1 is no longer
     * subscribed to*/
    if (group1_subscribed) return 1;
    if (group1_cb_callcount != 2) return 1;

    /* Expected result: during the whole test, non-relevant callbacks
     * are not called */
    if (group2_cb_callcount != 0) return 1;
    if (single1_cb_callcount != 2) return 1;
    if (single2_cb_callcount != 0) return 1;

    /* Test: unsubscribed properties change */
    context_provider_set_integer("Group1.Int", -8);
    context_provider_set_null("Group1.Bool");

    write_to_client("waitforchanged 3000\n");

    /* Expected result: the client doesn't get a signal */
    int mismatch = compare_output("Timeout\n");
    if (mismatch) return 1;

    return 0;
}

int test_resetting_values()
{
    /* Note: group1 and single1 were initialized with
     * reset-to-null=true, group2 and single2 with reset-to-null=false */

    /* Test: Command the client to subscribe */
    write_to_client("subscribe org.freedesktop.ContextKit.testProvider "
                    "Group1.Int Group2.Key2 Single1.Key Single2.Key\n");
    /* Expected result: the key is in the unknown list*/
    int mismatch = compare_output("Known keys: "
                                  "Unknown keys: Group1.Int Group2.Key2 "
                                  "Single1.Key Single2.Key \n");
    if (mismatch) return 1;

    /* Test: Set a value to the key */
    context_provider_set_integer("Group1.Int", 44);
    context_provider_set_integer("Group2.Key2", 100);
    context_provider_set_string("Single1.Key", "something");
    context_provider_set_string("Single2.Key", "else");
    write_to_client("waitforchanged 3000\n");

    /* Expected result: the client got the signal */
    mismatch = compare_output("Changed signal received, parameters: "
                              "Known keys: Group1.Int(int:44) "
                              "Group2.Key2(int:100) "
                              "Single1.Key(QString:something) "
                              "Single2.Key(QString:else) Unknown keys:  \n");
    if (mismatch) return 1;

    /* Test: Unsubscribe from the keys */
    write_to_client("unsubscribe org.freedesktop.ContextKit.testProvider "
                    "Group1.Int Group2.Key2 Single1.Key Single2.Key\n");

    /* Test: Subscribe again */
    write_to_client("subscribe org.freedesktop.ContextKit.testProvider "
                    "Group1.Int Group2.Key2 Single1.Key Single2.Key\n");
    /* Expected result: the key Group.Int is in the unknown list --
       even if we previously set a value. But the key Group2.Key2 has
       kept its value. The same for Single1.Key and Single2.Key.*/
    mismatch = compare_output("Known keys: Group2.Key2(int:100) "
                              "Single2.Key(QString:else) "
                              "Unknown keys: Group1.Int Single1.Key \n");
    if (mismatch) return 1;

    return 0;
}

int test_stopping()
{
    /* Test: Stop the service */
    context_provider_stop();

    /* Expected result: the client can no longer get a subscriber */
    write_to_client("getsubscriber "
                    "session org.freedesktop.ContextKit.testProvider\n");

    /* Expected result: the client got a correct subscriber path */
    int mismatch = compare_output("GetSubscriber error: invalid reply\n");
    if (mismatch) return 1;

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
    fprintf(stderr, "PASS\n");

    /* Uncomment this if you want to run the test program as a
       stand-alone provider:*/
    /* while (1) g_main_context_iteration(NULL, FALSE);*/

    fprintf(stderr, "Running test_get_subscriber... ");
    ret = test_get_subscriber();
    if (ret) {
        fprintf(stderr, "FAIL\n");
        return ret;
    }
    fprintf(stderr, "PASS\n");

    fprintf(stderr, "Running test_subscription... ");
    ret = test_subscription();
    if (ret) {
        fprintf(stderr, "FAIL\n");
        return ret;
    }
    fprintf(stderr, "PASS\n");

    fprintf(stderr, "Running test_callbacks... ");
    ret = test_callbacks();
    if (ret) {
        fprintf(stderr, "FAIL\n");
        return ret;
    }
    fprintf(stderr, "PASS\n");

    fprintf(stderr, "Running test_value_changes... ");
    ret = test_value_changes();
    if (ret) {
        fprintf(stderr, "FAIL\n");
        return ret;
    }
    fprintf(stderr, "PASS\n");

    fprintf(stderr, "Running test_unsubscription... ");
    ret = test_unsubscription();
    if (ret) {
        fprintf(stderr, "FAIL\n");
        return ret;
    }
    fprintf(stderr, "PASS\n");

    fprintf(stderr, "Running test_resetting_values... ");
    ret = test_resetting_values();
    if (ret) {
        fprintf(stderr, "FAIL\n");
        return ret;
    }
    fprintf(stderr, "PASS\n");


    fprintf(stderr, "Running test_stopping... ");
    ret = test_stopping();
    if (ret) {
        fprintf(stderr, "FAIL\n");
        return ret;
    }
    fprintf(stderr, "PASS\n");

    /* All tests successful */
    return 0;
}

int main(int argc, char **argv)
{
    GMainLoop *loop;
    int ret;

    /* Initialization */
    g_type_init ();
    loop = g_main_loop_new (NULL, FALSE);

    /* Start the client program */
    ret = socketpair(AF_UNIX, SOCK_STREAM, 0, tube);
    assert(ret == 0);

    if (fork() == 0) {
        /* child process */
        close(tube[1]);
        dup2(tube[0], 0); /* standard input from the stream pipe */
        dup2(tube[0], 1); /* standard output to the same descriptor */
        close(2); /* close stderr to reduce output when running the test */

        char* arg_list[] = {"client", NULL};
        execvp("client", arg_list);

        return 1;
    } else {
        /* parent process */
        close(tube[0]);
        sleep(1);

        /* Test cases */
        int ret = run_tests();

        /* Command the client to exit */
        close(tube[1]);

        /* Return the success / failure value */
        return ret;
    }
    return 0;
}
