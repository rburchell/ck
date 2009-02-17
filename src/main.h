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

#ifndef __SRC_MAIN_H__
#define __SRC_MAIN_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS


#define CONTEXT_KIT_TYPE_MAIN (context_kit_main_get_type ())
#define CONTEXT_KIT_MAIN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_KIT_TYPE_MAIN, ContextKitMain))
#define CONTEXT_KIT_MAIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), CONTEXT_KIT_TYPE_MAIN, ContextKitMainClass))
#define CONTEXT_KIT_IS_MAIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_KIT_TYPE_MAIN))
#define CONTEXT_KIT_IS_MAIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONTEXT_KIT_TYPE_MAIN))
#define CONTEXT_KIT_MAIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONTEXT_KIT_TYPE_MAIN, ContextKitMainClass))

typedef struct _ContextKitMain ContextKitMain;
typedef struct _ContextKitMainClass ContextKitMainClass;
typedef struct _ContextKitMainPrivate ContextKitMainPrivate;

struct _ContextKitMain {
	GObject parent_instance;
	ContextKitMainPrivate * priv;
};

struct _ContextKitMainClass {
	GObjectClass parent_class;
};


ContextKitMain* context_kit_main_construct (GType object_type);
ContextKitMain* context_kit_main_new (void);
void context_kit_main_run (ContextKitMain* self);
GType context_kit_main_get_type (void);


G_END_DECLS

#endif
