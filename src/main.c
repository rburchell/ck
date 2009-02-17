/* main.vala
 *
 * Copyright (C) 2008  Rob Taylor
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 *	Rob Taylor <rob.taylor@codethink.co.uk>
 */

#include <src/main.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include <string.h>
#include <manager.h>
#include <providers.h>
#include <provider_interface.h>
#include <stdio.h>
#include <signal.h>
#include "src/MCE.h"
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

typedef struct _DBusObjectVTable _DBusObjectVTable;

struct _DBusObjectVTable {
	void (*register_object) (DBusConnection*, const char*, void*);
};



enum  {
	CONTEXT_KIT_MAIN_DUMMY_PROPERTY
};
static GMainLoop* context_kit_main_loop = NULL;
static DBusGProxy* context_kit_main_bus = NULL;
guint _dynamic_RequestName0 (DBusGProxy* self, const char* param1, guint param2, GError** error);
static gboolean context_kit_main_start_manager (void);
static void context_kit_main_exit (gint sig);
static void _context_kit_main_exit_posix_signal_handler (gint sig_num);
static gint context_kit_main_main (char** args, int args_length1);
static gpointer context_kit_main_parent_class = NULL;
static void context_kit_main_finalize (GObject* obj);
static void _vala_dbus_register_object (DBusConnection* connection, const char* path, void* object);



/* Needs to be stored so that we get the NameOwnerChanged*/
ContextKitMain* context_kit_main_construct (GType object_type) {
	ContextKitMain * self;
	self = g_object_newv (object_type, 0, NULL);
	return self;
}


ContextKitMain* context_kit_main_new (void) {
	return context_kit_main_construct (CONTEXT_KIT_TYPE_MAIN);
}


guint _dynamic_RequestName0 (DBusGProxy* self, const char* param1, guint param2, GError** error) {
	guint result;
	dbus_g_proxy_call (self, "RequestName", error, G_TYPE_STRING, param1, G_TYPE_UINT, param2, G_TYPE_INVALID, G_TYPE_UINT, &result, G_TYPE_INVALID);
	if (*error) {
		return 0U;
	}
	return result;
}


static gboolean context_kit_main_start_manager (void) {
	GError * inner_error;
	inner_error = NULL;
	{
		DBusGConnection* connection;
		DBusGProxy* _tmp0;
		guint request_name_result;
		connection = dbus_g_bus_get (DBUS_BUS_SESSION, &inner_error);
		if (inner_error != NULL) {
			goto __catch0_g_error;
			goto __finally0;
		}
		_tmp0 = NULL;
		context_kit_main_bus = (_tmp0 = dbus_g_proxy_new_for_name (connection, "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus"), (context_kit_main_bus == NULL) ? NULL : (context_kit_main_bus = (g_object_unref (context_kit_main_bus), NULL)), _tmp0);
		/* try to register service in session bus*/
		request_name_result = _dynamic_RequestName0 (context_kit_main_bus, "org.freedesktop.ContextKit", (guint) 0, &inner_error);
		if (inner_error != NULL) {
			(connection == NULL) ? NULL : (connection = (dbus_g_connection_unref (connection), NULL));
			goto __catch0_g_error;
			goto __finally0;
		}
		if (request_name_result == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
			ContextProviderManager* manager;
			ContextKitMCEProvider* _tmp1;
			g_debug ("main.vala:44: Creating new Manager D-Bus service");
			manager = context_provider_manager_get_instance ();
			_tmp1 = NULL;
			context_provider_providers_register (context_provider_manager_get_providers (manager), (ContextProviderProvider*) (_tmp1 = context_kit_mce_provider_new ()));
			(_tmp1 == NULL) ? NULL : (_tmp1 = (g_object_unref (_tmp1), NULL));
			_vala_dbus_register_object (dbus_g_connection_get_connection (connection), "/org/freedesktop/ContextKit/Manager", (GObject*) manager);
			(manager == NULL) ? NULL : (manager = (g_object_unref (manager), NULL));
		} else {
			gboolean _tmp2;
			g_debug ("main.vala:52: Manager D-Bus service is already running");
			return (_tmp2 = FALSE, (connection == NULL) ? NULL : (connection = (dbus_g_connection_unref (connection), NULL)), _tmp2);
		}
		(connection == NULL) ? NULL : (connection = (dbus_g_connection_unref (connection), NULL));
	}
	goto __finally0;
	__catch0_g_error:
	{
		GError * e;
		e = inner_error;
		inner_error = NULL;
		{
			gboolean _tmp3;
			g_error ("main.vala:57: Oops %s", e->message);
			return (_tmp3 = FALSE, (e == NULL) ? NULL : (e = (g_error_free (e), NULL)), _tmp3);
		}
	}
	__finally0:
	if (inner_error != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
		g_clear_error (&inner_error);
		return FALSE;
	}
	return TRUE;
}


static void context_kit_main_exit (gint sig) {
	fprintf (stdout, "Exit contextd");
	g_main_loop_quit (context_kit_main_loop);
}


static void _context_kit_main_exit_posix_signal_handler (gint sig_num) {
	context_kit_main_exit (sig_num);
}


void context_kit_main_run (ContextKitMain* self) {
	GMainLoop* _tmp0;
	g_return_if_fail (self != NULL);
	_tmp0 = NULL;
	context_kit_main_loop = (_tmp0 = g_main_loop_new (NULL, FALSE), (context_kit_main_loop == NULL) ? NULL : (context_kit_main_loop = (g_main_loop_unref (context_kit_main_loop), NULL)), _tmp0);
	fprintf (stdout, "Hello, world!\n");
	context_kit_main_start_manager ();
	signal (SIGQUIT, _context_kit_main_exit_posix_signal_handler);
	g_main_loop_run (context_kit_main_loop);
}


static gint context_kit_main_main (char** args, int args_length1) {
	ContextKitMain* main;
	gint _tmp0;
	main = context_kit_main_new ();
	context_kit_main_run (main);
	return (_tmp0 = 0, (main == NULL) ? NULL : (main = (g_object_unref (main), NULL)), _tmp0);
}


int main (int argc, char ** argv) {
	g_type_init ();
	return context_kit_main_main (argv, argc);
}


static void context_kit_main_class_init (ContextKitMainClass * klass) {
	context_kit_main_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->finalize = context_kit_main_finalize;
}


static void context_kit_main_instance_init (ContextKitMain * self) {
}


static void context_kit_main_finalize (GObject* obj) {
	ContextKitMain * self;
	self = CONTEXT_KIT_MAIN (obj);
	G_OBJECT_CLASS (context_kit_main_parent_class)->finalize (obj);
}


GType context_kit_main_get_type (void) {
	static GType context_kit_main_type_id = 0;
	if (context_kit_main_type_id == 0) {
		static const GTypeInfo g_define_type_info = { sizeof (ContextKitMainClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) context_kit_main_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ContextKitMain), 0, (GInstanceInitFunc) context_kit_main_instance_init, NULL };
		context_kit_main_type_id = g_type_register_static (G_TYPE_OBJECT, "ContextKitMain", &g_define_type_info, 0);
	}
	return context_kit_main_type_id;
}


static void _vala_dbus_register_object (DBusConnection* connection, const char* path, void* object) {
	const _DBusObjectVTable * vtable;
	vtable = g_type_get_qdata (G_TYPE_FROM_INSTANCE (object), g_quark_from_static_string ("DBusObjectVTable"));
	if (vtable) {
		vtable->register_object (connection, path, object);
	} else {
		g_warning ("Object does not implement any D-Bus interface");
	}
}




