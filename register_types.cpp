#include "register_types.h"

#include "core/object/class_db.h"
#include "control3D.h"
#include "boxcontainer3D.h"
#include "richtextlabel3d.h"

void initialize_gui3D_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<Control3D>();
	ClassDB::register_class<BoxContainer3D>();
	ClassDB::register_class<VBoxContainer3D>();
	ClassDB::register_class<HBoxContainer3D>();
	ClassDB::register_class<ZBoxContainer3D>();
	ClassDB::register_class<RichTextLabel3D>();
}

void uninitialize_gui3D_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
   // Nothing to do here in this example.
}
