#include "interactable.h"

Interactable::Interactable() {
}

Interactable::~Interactable() {
}

void Interactable::_bind_methods() {
    // Virtual methods that can be called from GDScript
    ClassDB::bind_method(D_METHOD("can_interact_with", "interactor"), &Interactable::can_interact_with);
    ClassDB::bind_method(D_METHOD("on_hover_start", "interactor"), &Interactable::on_hover_start);
    ClassDB::bind_method(D_METHOD("on_hover_end", "interactor"), &Interactable::on_hover_end);
    ClassDB::bind_method(D_METHOD("on_activate", "interactor"), &Interactable::on_activate);
    ClassDB::bind_method(D_METHOD("on_end_activate", "interactor"), &Interactable::on_end_activate);
    ClassDB::bind_method(D_METHOD("on_interact", "interactor"), &Interactable::on_interact);
    ClassDB::bind_method(D_METHOD("on_end_interact", "interactor"), &Interactable::on_end_interact);

    // State methods
    ClassDB::bind_method(D_METHOD("is_enabled"), &Interactable::is_enabled);
    ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &Interactable::set_enabled);
    ClassDB::bind_method(D_METHOD("is_hovered"), &Interactable::is_hovered);
    ClassDB::bind_method(D_METHOD("is_activated"), &Interactable::is_activated);
    ClassDB::bind_method(D_METHOD("can_be_interacted"), &Interactable::can_be_interacted);
    ClassDB::bind_method(D_METHOD("set_can_be_interacted", "can_interact"), &Interactable::set_can_be_interacted);

    // Query methods
    ClassDB::bind_method(D_METHOD("is_hovered_by", "interactor"), &Interactable::is_hovered_by);
    ClassDB::bind_method(D_METHOD("is_activated_by", "interactor"), &Interactable::is_activated_by);
    // ClassDB::bind_method(D_METHOD("get_hovering_interactors"), &Interactable::get_hovering_interactors);
    // ClassDB::bind_method(D_METHOD("get_activating_interactors"), &Interactable::get_activating_interactors);

    // Properties
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "can_be_interacted"), "set_can_be_interacted", "can_be_interacted");

    // Signals
    ADD_SIGNAL(MethodInfo("hover_started", PropertyInfo(Variant::OBJECT, "interactor")));
    ADD_SIGNAL(MethodInfo("hover_ended", PropertyInfo(Variant::OBJECT, "interactor")));
    ADD_SIGNAL(MethodInfo("activated", PropertyInfo(Variant::OBJECT, "interactor")));
    ADD_SIGNAL(MethodInfo("deactivated", PropertyInfo(Variant::OBJECT, "interactor")));
    ADD_SIGNAL(MethodInfo("interacted", PropertyInfo(Variant::OBJECT, "interactor")));
    ADD_SIGNAL(MethodInfo("interaction_ended", PropertyInfo(Variant::OBJECT, "interactor")));
}

void Interactable::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            // Add to interactables group
            add_to_group("interactables");
        } break;
    }
}

bool Interactable::can_interact_with(Interactor* interactor) {
    if (!_is_enabled || !_can_be_interacted || !interactor) {
        return false;
    }
    
    return _validate_interaction(interactor);
}

void Interactable::on_hover_start(Interactor* interactor) {
    if (!can_interact_with(interactor)) return;
    
    _add_hovering_interactor(interactor);
    emit_signal("hover_started", interactor);
}

void Interactable::on_hover_end(Interactor* interactor) {
    _remove_hovering_interactor(interactor);
    emit_signal("hover_ended", interactor);
}

void Interactable::on_activate(Interactor* interactor) {
    if (!can_interact_with(interactor)) return;
    
    _add_activating_interactor(interactor);
    emit_signal("activated", interactor);
}

void Interactable::on_end_activate(Interactor* interactor) {
    _remove_activating_interactor(interactor);
    emit_signal("deactivated", interactor);
}

void Interactable::on_interact(Interactor* interactor) {
    if (!can_interact_with(interactor)) return;
    
    emit_signal("interacted", interactor);
}

void Interactable::on_end_interact(Interactor* interactor) {
    emit_signal("interaction_ended", interactor);
}

// State queries
bool Interactable::is_enabled() const {
    return _is_enabled;
}

void Interactable::set_enabled(bool p_enabled) {
    _is_enabled = p_enabled;
}

bool Interactable::is_hovered() const {
    return _hovering_interactors.size() > 0;
}

bool Interactable::is_activated() const {
    return _activating_interactors.size() > 0;
}

bool Interactable::can_be_interacted() const {
    return _can_be_interacted;
}

void Interactable::set_can_be_interacted(bool p_can_interact) {
    _can_be_interacted = p_can_interact;
}

// Interactor management
bool Interactable::is_hovered_by(Interactor* interactor) const {
    return _hovering_interactors.has(interactor);
}

bool Interactable::is_activated_by(Interactor* interactor) const {
    return _activating_interactors.has(interactor);
}

Vector<Interactor*> Interactable::get_hovering_interactors() const {
    return _hovering_interactors;
}

Vector<Interactor*> Interactable::get_activating_interactors() const {
    return _activating_interactors;
}

// Internal state management
void Interactable::_add_hovering_interactor(Interactor* interactor) {
    if (!_hovering_interactors.has(interactor)) {
        _hovering_interactors.push_back(interactor);
    }
}

void Interactable::_remove_hovering_interactor(Interactor* interactor) {
    _hovering_interactors.erase(interactor);
}

void Interactable::_add_activating_interactor(Interactor* interactor) {
    if (!_activating_interactors.has(interactor)) {
        _activating_interactors.push_back(interactor);
    }
}

void Interactable::_remove_activating_interactor(Interactor* interactor) {
    _activating_interactors.erase(interactor);
}

bool Interactable::_validate_interaction(Interactor* interactor) {
    // Default validation - override in derived classes for custom logic
    return true;
}
