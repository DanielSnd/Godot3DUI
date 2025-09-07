#include "poke_interactable.h"
 
PokeInteractable::PokeInteractable() {
}
 
PokeInteractable::~PokeInteractable() {
}
 
void PokeInteractable::_bind_methods() {
    // Poke-specific methods
    ClassDB::bind_method(D_METHOD("can_be_poked_by", "poke_interactor"), &PokeInteractable::can_be_poked_by);
    ClassDB::bind_method(D_METHOD("on_poke_start", "poke_interactor", "contact_point", "surface_normal"), &PokeInteractable::on_poke_start);
    ClassDB::bind_method(D_METHOD("on_poke_update", "poke_interactor", "poke_depth", "contact_point"), &PokeInteractable::on_poke_update);
    ClassDB::bind_method(D_METHOD("on_poke_end", "poke_interactor", "max_depth_reached"), &PokeInteractable::on_poke_end);
 
    // State query methods
    ClassDB::bind_method(D_METHOD("is_being_poked"), &PokeInteractable::is_being_poked);
    ClassDB::bind_method(D_METHOD("is_being_poked_by", "poke_interactor"), &PokeInteractable::is_being_poked_by);
    // ClassDB::bind_method(D_METHOD("get_poking_interactors"), &PokeInteractable::get_poking_interactors);
    ClassDB::bind_method(D_METHOD("get_total_poke_depth"), &PokeInteractable::get_total_poke_depth);
    ClassDB::bind_method(D_METHOD("get_max_poke_depth_by", "poke_interactor"), &PokeInteractable::get_max_poke_depth_by);
    ClassDB::bind_method(D_METHOD("get_poke_contact_point", "poke_interactor"), &PokeInteractable::get_poke_contact_point);
 
    // Configuration methods
    ClassDB::bind_method(D_METHOD("set_poke_response_threshold", "threshold"), &PokeInteractable::set_poke_response_threshold);
    ClassDB::bind_method(D_METHOD("get_poke_response_threshold"), &PokeInteractable::get_poke_response_threshold);
    
    ClassDB::bind_method(D_METHOD("set_max_poke_depth", "depth"), &PokeInteractable::set_max_poke_depth);
    ClassDB::bind_method(D_METHOD("get_max_poke_depth"), &PokeInteractable::get_max_poke_depth);
    
    ClassDB::bind_method(D_METHOD("set_consume_poke", "consume"), &PokeInteractable::set_consume_poke);
    ClassDB::bind_method(D_METHOD("get_consume_poke"), &PokeInteractable::get_consume_poke);
    
    ClassDB::bind_method(D_METHOD("set_surface", "surface"), &PokeInteractable::set_surface);
    ClassDB::bind_method(D_METHOD("get_surface"), &PokeInteractable::get_surface);
 
    // Properties
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "poke_response_threshold", PROPERTY_HINT_RANGE, "0.001,0.1,0.001"), "set_poke_response_threshold", "get_poke_response_threshold");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_poke_depth", PROPERTY_HINT_RANGE, "0.001,0.2,0.001"), "set_max_poke_depth", "get_max_poke_depth");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "consume_poke"), "set_consume_poke", "get_consume_poke");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "surface", PROPERTY_HINT_NODE_TYPE, "Surface3D"), "set_surface", "get_surface");
 
    // Poke-specific signals
    ADD_SIGNAL(MethodInfo("poke_started", PropertyInfo(Variant::OBJECT, "poke_interactor"), PropertyInfo(Variant::VECTOR3, "contact_point"), PropertyInfo(Variant::VECTOR3, "surface_normal")));
    ADD_SIGNAL(MethodInfo("poke_updated", PropertyInfo(Variant::OBJECT, "poke_interactor"), PropertyInfo(Variant::FLOAT, "poke_depth"), PropertyInfo(Variant::VECTOR3, "contact_point")));
    ADD_SIGNAL(MethodInfo("poke_ended", PropertyInfo(Variant::OBJECT, "poke_interactor"), PropertyInfo(Variant::FLOAT, "max_depth_reached")));
}

bool PokeInteractable::can_interact_with(Interactor* interactor) {
    if (!Interactable::can_interact_with(interactor)) {
        return false;
    }
    
    PokeInteractor* poke_interactor = Object::cast_to<PokeInteractor>(interactor);
    if (poke_interactor) {
        return can_be_poked_by(poke_interactor);
    }
    
    return true; // Allow non-poke interactors
}

void PokeInteractable::on_hover_start(Interactor* interactor) {
    Interactable::on_hover_start(interactor);
    
    PokeInteractor* poke_interactor = Object::cast_to<PokeInteractor>(interactor);
    if (poke_interactor) {
        // Update poke state during hover
        _update_poke_state(poke_interactor);
    }
}

void PokeInteractable::on_hover_end(Interactor* interactor) {
    Interactable::on_hover_end(interactor);
    
    PokeInteractor* poke_interactor = Object::cast_to<PokeInteractor>(interactor);
    if (poke_interactor && is_being_poked_by(poke_interactor)) {
        // End poke if we're no longer hovering
        PokeState* state = get_poke_state(poke_interactor);
        float max_depth = state ? state->max_depth_reached : 0.0f;
        on_poke_end(poke_interactor, max_depth);
    }
}
 
void PokeInteractable::on_activate(Interactor* interactor) {
    Interactable::on_activate(interactor);
    
    PokeInteractor* poke_interactor = Object::cast_to<PokeInteractor>(interactor);
    if (poke_interactor && can_be_poked_by(poke_interactor)) {
        Vector3 contact_point = _get_contact_point(poke_interactor);
        Vector3 surface_normal = _get_surface_normal(poke_interactor);
        on_poke_start(poke_interactor, contact_point, surface_normal);
    }
}
 
void PokeInteractable::on_end_activate(Interactor* interactor) {
    Interactable::on_end_activate(interactor);
    
    PokeInteractor* poke_interactor = Object::cast_to<PokeInteractor>(interactor);
    if (poke_interactor && is_being_poked_by(poke_interactor)) {
        PokeState* state = get_poke_state(poke_interactor);
        float max_depth = state ? state->max_depth_reached : 0.0f;
        on_poke_end(poke_interactor, max_depth);
    }
}
 
bool PokeInteractable::can_be_poked_by(PokeInteractor* poke_interactor) {
    if (!poke_interactor || !is_enabled() || !can_be_interacted()) {
        return false;
    }
    
    // Check if we require a surface and have one
    if (_requires_surface && !_surface) {
        return false;
    }
    
    // Check if we're penetrating the surface
    if (!_check_surface_penetration(poke_interactor)) {
        return false;
    }
    
    return _validate_poke_interaction(poke_interactor);
}
 
void PokeInteractable::on_poke_start(PokeInteractor* poke_interactor, const Vector3& contact_point, const Vector3& surface_normal) {
    if (!can_be_poked_by(poke_interactor)) return;
    
    // Add to poking interactors
    if (!_poking_interactors.has(poke_interactor)) {
        _poking_interactors.push_back(poke_interactor);
    }
    
    // Initialize poke state
    PokeState state;
    state.contact_point = contact_point;
    state.surface_normal = surface_normal;
    state.poke_depth = _calculate_poke_depth(poke_interactor);
    state.max_depth_reached = state.poke_depth;
    state.is_active = true;
    
    _poke_states[poke_interactor] = state;
    
    emit_signal("poke_started", poke_interactor, contact_point, surface_normal);
}
 
void PokeInteractable::on_poke_update(PokeInteractor* poke_interactor, float poke_depth, const Vector3& contact_point) {
    if (!is_being_poked_by(poke_interactor)) return;
    
    PokeState* state = get_poke_state(poke_interactor);
    if (state) {
        state->poke_depth = poke_depth;
        state->contact_point = contact_point;
        state->max_depth_reached = MAX(state->max_depth_reached, poke_depth);
        
        _poke_states[poke_interactor] = *state;
    }
    
    emit_signal("poke_updated", poke_interactor, poke_depth, contact_point);
}
 
void PokeInteractable::on_poke_end(PokeInteractor* poke_interactor, float max_depth_reached) {
    if (!is_being_poked_by(poke_interactor)) return;
    
    // Remove from poking interactors
    _poking_interactors.erase(poke_interactor);
    
    // Clean up poke state
    _poke_states.erase(poke_interactor);
    
    emit_signal("poke_ended", poke_interactor, max_depth_reached);
}
 
// State queries
bool PokeInteractable::is_being_poked() const {
    return _poking_interactors.size() > 0;
}
 
bool PokeInteractable::is_being_poked_by(PokeInteractor* poke_interactor) const {
    return _poking_interactors.has(poke_interactor);
}
 
Vector<PokeInteractor*> PokeInteractable::get_poking_interactors() const {
    return _poking_interactors;
}
 
float PokeInteractable::get_total_poke_depth() const {
    float total = 0.0f;
    for (int i = 0; i < _poking_interactors.size(); i++) {
        PokeState* state = get_poke_state(_poking_interactors[i]);
        if (state) {
            total += state->poke_depth;
        }
    }
    return total;
}
 
float PokeInteractable::get_max_poke_depth_by(PokeInteractor* poke_interactor) const {
    PokeState* state = get_poke_state(poke_interactor);
    return state ? state->max_depth_reached : 0.0f;
}
 
Vector3 PokeInteractable::get_poke_contact_point(PokeInteractor* poke_interactor) const {
    PokeState* state = get_poke_state(poke_interactor);
    return state ? state->contact_point : Vector3();
}
 
// Configuration getters/setters
void PokeInteractable::set_poke_response_threshold(float p_threshold) {
    _poke_response_threshold = MAX(0.001f, p_threshold);
}
 
float PokeInteractable::get_poke_response_threshold() const {
    return _poke_response_threshold;
}
 
void PokeInteractable::set_max_poke_depth(float p_depth) {
    _max_poke_depth = MAX(0.001f, p_depth);
}
 
float PokeInteractable::get_max_poke_depth() const {
    return _max_poke_depth;
}

void PokeInteractable::set_consume_poke(bool p_consume) {
    _consume_poke = p_consume;
}
 
bool PokeInteractable::get_consume_poke() const {
    return _consume_poke;
}
 
void PokeInteractable::set_surface(Surface3D* p_surface) {
    _surface = p_surface;
}
 
Surface3D* PokeInteractable::get_surface() const {
    return _surface;
}
 
// Protected methods
bool PokeInteractable::_validate_poke_interaction(PokeInteractor* poke_interactor) {
    // Check poke depth threshold
    float current_depth = _calculate_poke_depth(poke_interactor);
    if (current_depth < _poke_response_threshold) {
        return false;
    }
    
    // Check max poke depth
    if (current_depth > _max_poke_depth) {
        return false;
    }
    
    return true;
}
 
void PokeInteractable::_update_poke_state(PokeInteractor* poke_interactor) {
    if (!is_being_poked_by(poke_interactor)) return;
    
    float current_depth = _calculate_poke_depth(poke_interactor);
    Vector3 contact_point = _get_contact_point(poke_interactor);
    
    on_poke_update(poke_interactor, current_depth, contact_point);
}

float PokeInteractable::_calculate_poke_depth(PokeInteractor* poke_interactor) {
    if (!poke_interactor || !_surface) {
        return 0.0f;
    }
    
    Vector3 poke_position = poke_interactor->get_global_position();
    Surface3D::SurfaceHit hit;
    
    if (_surface->closest_surface_point(poke_position, hit)) {
        if (hit.valid) {
            // Calculate penetration depth
            // If we're very close to or past the surface, calculate how much we've penetrated
            float poke_radius = poke_interactor->get_poke_radius();
            return MAX(0.0f, poke_radius - hit.distance);
        }
    }
    
    return 0.0f;
}

bool PokeInteractable::_check_surface_penetration(PokeInteractor* poke_interactor) {
    if (!poke_interactor || !_surface) {
        return false;
    }
    
    Vector3 poke_position = poke_interactor->get_global_position();
    Surface3D::SurfaceHit hit;
    
    if (_surface->closest_surface_point(poke_position, hit)) {
        // Check if we're within poke threshold distance
        float poke_threshold = poke_interactor->get_poke_threshold();
        return hit.valid && hit.distance <= poke_threshold;
    }
    
    return false;
}

Vector3 PokeInteractable::_get_contact_point(PokeInteractor* poke_interactor) {
    if (!poke_interactor || !_surface) {
        return Vector3();
    }
    
    Vector3 poke_position = poke_interactor->get_global_position();
    Surface3D::SurfaceHit hit;
    
    if (_surface->closest_surface_point(poke_position, hit)) {
        return hit.valid ? hit.point : Vector3();
    }
    
    return Vector3();
}

Vector3 PokeInteractable::_get_surface_normal(PokeInteractor* poke_interactor) {
    if (!poke_interactor || !_surface) {
        return Vector3(0, 1, 0); // Default up vector
    }
    
    Vector3 poke_position = poke_interactor->get_global_position();
    Surface3D::SurfaceHit hit;
    
    if (_surface->closest_surface_point(poke_position, hit)) {
        return hit.valid ? hit.normal : Vector3(0, 1, 0);
    }
    
    return Vector3(0, 1, 0);
}

PokeInteractable::PokeState* PokeInteractable::get_poke_state(PokeInteractor* poke_interactor) const {
    if (!poke_interactor) return nullptr;
    
    return const_cast<PokeState*>(_poke_states.getptr(poke_interactor));
}
