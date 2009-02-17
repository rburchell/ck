
#ifndef __SRC_MCE_H__
#define __SRC_MCE_H__

#include <glib.h>
#include <glib-object.h>
#include <provider_interface.h>
#include <stdlib.h>
#include <string.h>
#include <string_set.h>
#include <gee/arraylist.h>

G_BEGIN_DECLS

typedef struct _ContextKitMCEDeviceOrientation ContextKitMCEDeviceOrientation;

#define CONTEXT_KIT_MCE_TYPE_DEVICE_ORIENTATION (context_kit_mce_device_orientation_get_type ())

#define CONTEXT_KIT_MCE_TYPE_PROVIDER (context_kit_mce_provider_get_type ())
#define CONTEXT_KIT_MCE_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONTEXT_KIT_MCE_TYPE_PROVIDER, ContextKitMCEProvider))
#define CONTEXT_KIT_MCE_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), CONTEXT_KIT_MCE_TYPE_PROVIDER, ContextKitMCEProviderClass))
#define CONTEXT_KIT_MCE_IS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONTEXT_KIT_MCE_TYPE_PROVIDER))
#define CONTEXT_KIT_MCE_IS_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONTEXT_KIT_MCE_TYPE_PROVIDER))
#define CONTEXT_KIT_MCE_PROVIDER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CONTEXT_KIT_MCE_TYPE_PROVIDER, ContextKitMCEProviderClass))

typedef struct _ContextKitMCEProvider ContextKitMCEProvider;
typedef struct _ContextKitMCEProviderClass ContextKitMCEProviderClass;
typedef struct _ContextKitMCEProviderPrivate ContextKitMCEProviderPrivate;

struct _ContextKitMCEDeviceOrientation {
	char* rotation;
	char* stand;
	char* facing;
	gint x;
	gint y;
	gint z;
};

struct _ContextKitMCEProvider {
	GObject parent_instance;
	ContextKitMCEProviderPrivate * priv;
};

struct _ContextKitMCEProviderClass {
	GObjectClass parent_class;
};


void context_kit_mce_device_orientation_copy (const ContextKitMCEDeviceOrientation* self, ContextKitMCEDeviceOrientation* dest);
void context_kit_mce_device_orientation_destroy (ContextKitMCEDeviceOrientation* self);
ContextKitMCEDeviceOrientation* context_kit_mce_device_orientation_dup (const ContextKitMCEDeviceOrientation* self);
void context_kit_mce_device_orientation_free (ContextKitMCEDeviceOrientation* self);
GType context_kit_mce_device_orientation_get_type (void);
#define CONTEXT_KIT_MCE_PROVIDER_key_edge_up "Context.Device.Orientation.edgeUp"
#define CONTEXT_KIT_MCE_PROVIDER_key_facing_up "Context.Device.Orientation.facingUp"
#define CONTEXT_KIT_MCE_PROVIDER_key_display_state "Context.Device.Display.displayState"
#define CONTEXT_KIT_MCE_PROVIDER_key_profile_name "Context.Device.???.profileName"
#define CONTEXT_KIT_MCE_PROVIDER_key_is_flight_mode "Context.Environment.Connection.CurrentData.isFlightMode"
#define CONTEXT_KIT_MCE_PROVIDER_key_is_emergency_mode "Context.Environment.Connection.CurrentData.isEmergencyMode"
#define CONTEXT_KIT_MCE_PROVIDER_key_in_active_use "Context.Device.Usage.inActiveUse"
ContextKitMCEProvider* context_kit_mce_provider_construct (GType object_type);
ContextKitMCEProvider* context_kit_mce_provider_new (void);
GType context_kit_mce_provider_get_type (void);


G_END_DECLS

#endif
