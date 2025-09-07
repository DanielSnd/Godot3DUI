#include "poke_interactor.h"
#include "scene/main/scene_tree.h"
#include "../interactables/interactable.h"

PokeInteractor::PokeInteractor() {
    last_position = Vector3();
    velocity = Vector3();
}

PokeInteractor::~PokeInteractor() {
}

void PokeInteractor::_bind_methods() {
    // Property bindings
    ClassDB::bind_method(D_METHOD("set_poke_radius", "radius"), &PokeInteractor::set_poke_radius);
    ClassDB::bind_method(D_METHOD("get_poke_radius"), &PokeInteractor::get_poke_radius);
    
    ClassDB::bind_method(D_METHOD("set_poke_threshold", "threshold"), &PokeInteractor::set_poke_threshold);
    ClassDB::bind_method(D_METHOD("get_poke_threshold"), &PokeInteractor::get_poke_threshold);
    
    ClassDB::bind_method(D_METHOD("set_velocity_threshold", "threshold"), &PokeInteractor::set_velocity_threshold);
    ClassDB::bind_method(D_METHOD("get_velocity_threshold"), &PokeInteractor::get_velocity_threshold);
    
    ClassDB::bind_method(D_METHOD("set_max_poke_depth", "depth"), &PokeInteractor::set_max_poke_depth);
    ClassDB::bind_method(D_METHOD("get_max_poke_depth"), &PokeInteractor::get_max_poke_depth);
    
    ClassDB::bind_method(D_METHOD("set_interaction_search_radius", "radius"), &PokeInteractor::set_interaction_search_radius);
    ClassDB::bind_method(D_METHOD("get_interaction_search_radius"), &PokeInteractor::get_interaction_search_radius);
    
    // State query methods
    ClassDB::bind_method(D_METHOD("get_is_poking"), &PokeInteractor::get_is_poking);
    ClassDB::bind_method(D_METHOD("get_poke_velocity"), &PokeInteractor::get_poke_velocity);

    // Properties
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "poke_radius", PROPERTY_HINT_RANGE, "0.001,0.1,0.001"), "set_poke_radius", "get_poke_radius");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "poke_threshold", PROPERTY_HINT_RANGE, "0.001,0.05,0.001"), "set_poke_threshold", "get_poke_threshold");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "velocity_threshold", PROPERTY_HINT_RANGE, "0.01,1.0,0.01"), "set_velocity_threshold", "get_velocity_threshold");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_poke_depth", PROPERTY_HINT_RANGE, "0.001,0.1,0.001"), "set_max_poke_depth", "get_max_poke_depth");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "interaction_search_radius", PROPERTY_HINT_RANGE, "0.01,0.5,0.01"), "set_interaction_search_radius", "get_interaction_search_radius");
}

void PokeInteractor::before() {
    Interactor::before();
    _update_velocity();
}

void PokeInteractor::process() {
    Interactor::process();
}

void PokeInteractor::_update_velocity() {
    Vector3 current_position = get_global_position();
    float delta = get_process_delta_time();
    if (delta > 0.0f) {
        velocity = (current_position - last_position) / delta;
    }
    last_position = current_position;
}

void PokeInteractor::find_potential_interactable() {
    Vector<Interactable*> nearby_interactables = _get_nearby_interactables();
    
    Interactable* closest_interactable = nullptr;
    float closest_distance = INFINITY;
    Vector3 poke_position = get_global_position();
    
    for (int i = 0; i < nearby_interactables.size(); i++) {
        Interactable* interactable = nearby_interactables[i];
        if (!interactable) continue;
        
        // Let the interactable decide if it can interact with this poke interactor
        if (interactable->can_interact_with(this)) {
            float distance = poke_position.distance_to(interactable->get_global_position());
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_interactable = interactable;
            }
        }
    }
    
    if (closest_interactable) {
        potential_interactable = Variant(closest_interactable);
        has_potential_interactable = true;
    } else {
        potential_interactable = Variant();
        has_potential_interactable = false;
    }
}

Vector<Interactable*> PokeInteractor::_get_nearby_interactables() {
    Vector<Interactable*> nearby_interactables;
    
    // Get all interactables in the scene
    // This is a simplified approach - in a real implementation you might want
    // to use a spatial partitioning system or have an InteractorManager handle this
    SceneTree* tree = get_tree();
    if (!tree) return nearby_interactables;
    
    Vector3 poke_position = get_global_position();
    
    // Find all Interactable nodes in the scene
    // This is a basic implementation - you might want to optimize this
    Node* root = tree->get_root();
    if (root) {
        _find_interactables_recursive(root, nearby_interactables, poke_position);
    }
    
    return nearby_interactables;
}

void PokeInteractor::_find_interactables_recursive(Node* node, Vector<Interactable*>& interactables, const Vector3& poke_position) {
    if (!node) return;
    
    // Check if this node is an Interactable
    Interactable* interactable = Object::cast_to<Interactable>(node);
    if (interactable && interactable->is_enabled()) {
        float distance = poke_position.distance_to(interactable->get_global_position());
        if (distance <= interaction_search_radius) {
            interactables.push_back(interactable);
        }
    }
    
    // Recursively check children
    for (int i = 0; i < node->get_child_count(); i++) {
        _find_interactables_recursive(node->get_child(i), interactables, poke_position);
    }
}

bool PokeInteractor::get_should_start_hover() {
    return has_potential_interactable && !has_interactable;
}

bool PokeInteractor::get_should_activate() {
    if (!has_interactable || is_poking) return false;
    
    // Check if we have sufficient velocity and meet poke conditions
    return velocity.length() > velocity_threshold && _check_poke_conditions();
}

bool PokeInteractor::get_should_end_activate() {
    if (!is_poking) return false;
    
    // End activation if velocity is too low or we no longer meet poke conditions
    return velocity.length() < velocity_threshold * 0.5f || !_check_poke_conditions();
}

bool PokeInteractor::_check_poke_conditions() {
    if (!has_interactable) return false;
    
    // The interactable should handle the specific poke validation logic
    // This is just a basic check for velocity and general conditions
    return velocity.length() > velocity_threshold;
}

void PokeInteractor::on_activate() {
    is_poking = true;
    Interactor::on_activate();
}

void PokeInteractor::on_end_activate() {
    is_poking = false;
    Interactor::on_end_activate();
}

// Getters
bool PokeInteractor::get_is_poking() const {
    return is_poking;
}

Vector3 PokeInteractor::get_poke_velocity() const {
    return velocity;
}

// Setters and getters
void PokeInteractor::set_poke_radius(float p_radius) {
    poke_radius = MAX(0.001f, p_radius);
}

float PokeInteractor::get_poke_radius() const {
    return poke_radius;
}

void PokeInteractor::set_poke_threshold(float p_threshold) {
    poke_threshold = MAX(0.001f, p_threshold);
}

float PokeInteractor::get_poke_threshold() const {
    return poke_threshold;
}

void PokeInteractor::set_velocity_threshold(float p_threshold) {
    velocity_threshold = MAX(0.01f, p_threshold);
}

float PokeInteractor::get_velocity_threshold() const {
    return velocity_threshold;
}

void PokeInteractor::set_max_poke_depth(float p_depth) {
    max_poke_depth = MAX(0.001f, p_depth);
}

float PokeInteractor::get_max_poke_depth() const {
    return max_poke_depth;
}

void PokeInteractor::set_interaction_search_radius(float p_radius) {
    interaction_search_radius = MAX(0.01f, p_radius);
}

float PokeInteractor::get_interaction_search_radius() const {
    return interaction_search_radius;
}
