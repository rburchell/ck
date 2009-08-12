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

    int temp;
    /* Write the command to the client */
    temp = dprintf(tube[1], command);

    /* Process the events until we can read from the client */
    do {
        /*fprintf(stderr, "processing\n");*/
        g_main_context_iteration(NULL, TRUE);

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
    /*fprintf(stderr, "%s\n", line_from_client);*/

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
    context_provider_install_key(single1_key, FALSE,
                                 single1_cb, NULL);
    context_provider_install_key(single2_key, FALSE,
                                 single2_cb, NULL);

    return 0;
}

int test_get_subscriber()
{
    /* Test: Command the client to execute GetSubscriber over DBus. */
    write_to_client("getsubscriber session org.freedesktop.ContextKit.testProvider\n");

    /* Expected result: the client got a correct subscriber path */
    int mismatch = strncmp(line_from_client,
                           "GetSubscriber returned "
                           "/org/freedesktop/ContextKit/Subscriber/0",
                           chars_read-1);
    if (mismatch) return 1;
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

    fprintf(stderr, "Running test_get_subscriber... ");
    ret = test_get_subscriber();
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
        dup2(tube[0], 0);   /* standard input from the stream pipe */
        dup2(tube[0], 1);   /* standard output to the same descriptor */

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
