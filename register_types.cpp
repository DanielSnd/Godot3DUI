#include "register_types.h"

#include "core/object/class_db.h"
#include "control3D.h"
#include "boxcontainer3D.h"
#include "richtextlabel3d.h"
#include "surface3d.h"
#include "button3d.h"
#include "interactors/interactor.h"
#include "interactors/interactor_manager.h"
#include "interactables/interactable.h"
#include "interactables/poke_interactable.h"

#include "fast_text_shared.h"
#include "fast_text.h"

void initialize_gui3D_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<Control3D>();
	ClassDB::register_class<FastTextSharedResource>();
	ClassDB::register_class<FastText>();
	ClassDB::register_class<BoxContainer3D>();
	ClassDB::register_class<VBoxContainer3D>();
	ClassDB::register_class<HBoxContainer3D>();
	ClassDB::register_class<ZBoxContainer3D>();
	ClassDB::register_class<RichTextLabel3D>();
	ClassDB::register_class<PlaneSurface3D>();
	ClassDB::register_class<LimitedPlaneSurface3D>();
	ClassDB::register_class<Button3D>();
	ClassDB::register_class<Interactor>();
	ClassDB::register_class<InteractorManager>();
	ClassDB::register_class<Interactable>();
	ClassDB::register_class<PokeInteractable>();
	ClassDB::register_class<PokeInteractor>();
}

void uninitialize_gui3D_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
   // Nothing to do here in this example.
}
