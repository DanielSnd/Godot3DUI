#include "interactor.h"

void Interactor::_bind_methods() {
    BIND_ENUM_CONSTANT(INTERACTOR_EVENT_CANCEL);
    BIND_ENUM_CONSTANT(INTERACTOR_EVENT_ENABLE);
    BIND_ENUM_CONSTANT(INTERACTOR_EVENT_DISABLE);
    BIND_ENUM_CONSTANT(INTERACTOR_EVENT_HOVER_START);
    BIND_ENUM_CONSTANT(INTERACTOR_EVENT_HOVER_END);
    BIND_ENUM_CONSTANT(INTERACTOR_EVENT_ACTIVATE);
    BIND_ENUM_CONSTANT(INTERACTOR_EVENT_END_ACTIVATE);
    BIND_ENUM_CONSTANT(INTERACTOR_EVENT_INTERACT);
    BIND_ENUM_CONSTANT(INTERACTOR_EVENT_END_INTERACT);
    BIND_ENUM_CONSTANT(INTERACTOR_EVENT_MOVEMENT);
}

void Interactor::_notification(int p_what) {
    if (p_what == NOTIFICATION_READY && !Engine::get_singleton()->is_editor_hint()) {
        InteractorManager::EnsureInteractorManagerExists();
    }
}

// We start the interactor processing from interaction manager.
// We go over interactors finding interactors that aren't a part of a group/have no parent interactor.
// Once we have an interactor that has no parent one, we will process that one.
// Essentially when processing, unless we already have an active interactor, we want to see if the interactor
// can be hovering an interactable, or selecting an interactable.
// If we're talking about a group, we need to then check the ones under the group, if not, we need to check that specific one.

// So let's say we have a ray interactor.
// If we have a ray interactor, it's hovering if the raycast hits an interactable that can be interacted with by a ray interactor.
// Then we have a valid hover and we can switch to hovering state.
// If we're in hovering state, and the ray interactor has the button pressed (activation intention?), then we can switch to activated state.
// If we're in activated state and the ray interactor has the button released, we can switch to hovering state.
// If we're in hovering state and the ray interactor's raycast is not hitting the interactable then we switch to no hover state.

// Let's say we have a best hover group interactor. And under that there's a poke interactor and a ray interactor.
// If the poke interactor is in an active hover state, it'll stop there and won't even check the ray interactor, it'll enter hover state there.
// If it's already in a hover state, it'll just do the one that's already hovering, same for selecting.
// 

Interactor::Interactor() {
    // Constructor
}

Interactor::~Interactor() {
    // Destructor
}

// Main process methods - virtual base implementations
void Interactor::before() {
    // Override in derived classes
}

void Interactor::process() {
    // Override in derived classes
}

void Interactor::after() {
    // Override in derived classes
}

// Interactable discovery
void Interactor::find_potential_interactable() {
    // Override in derived classes
    // Default implementation does nothing
}

bool Interactor::get_has_potential_interactable() const {
    return has_potential_interactable;
}

Variant Interactor::get_potential_interactable() const {
    return potential_interactable;
}

// Hover system
bool Interactor::get_should_start_hover() {
    // Override in derived classes
    return false;
}

void Interactor::on_hover() {
    // Override in derived classes
}

bool Interactor::get_should_end_hover() {
    // Override in derived classes
    return false;
}

void Interactor::on_hover_end() {
    // Override in derived classes
}

// Activation system
bool Interactor::get_should_activate() {
    // Override in derived classes
    return false;
}

void Interactor::on_activate() {
    // Override in derived classes
    has_activated_interactable = true;
}

bool Interactor::get_should_end_activate() {
    // Override in derived classes
    return false;
}

void Interactor::on_end_activate() {
    // Override in derived classes
    has_activated_interactable = false;
}

bool Interactor::get_is_part_of_group() const {
    return is_part_of_group;
}

// State queries
bool Interactor::get_has_interactable() const {
    return has_interactable;
}

bool Interactor::get_has_activated_interactable() const {
    return has_activated_interactable;
}
