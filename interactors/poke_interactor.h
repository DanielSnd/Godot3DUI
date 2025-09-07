#ifndef POKE_INTERACTOR_H
#define POKE_INTERACTOR_H

#include "interactor.h"

class InteractorManager;
class Interactable;
class PokeInteractor : public Interactor {
    GDCLASS(PokeInteractor, Interactor)

protected:
    static void _bind_methods();

private:
    Vector3 last_position;
    Vector3 velocity;
    bool is_poking = false;
    
    // Configuration properties
    float poke_radius = 0.01f;  // 1cm radius
    float poke_threshold = 0.005f;  // 5mm penetration to trigger
    float velocity_threshold = 0.1f;  // Minimum velocity for poke
    float max_poke_depth = 0.02f;  // Maximum penetration depth (2cm)
    float interaction_search_radius = 0.05f;  // How far to search for interactables (5cm)
    
    // Internal methods
    void _update_velocity();
    bool _check_poke_conditions();
    Vector<Interactable*> _get_nearby_interactables();
    void _find_interactables_recursive(Node* node, Vector<Interactable*>& interactables, const Vector3& poke_position);

public:
    PokeInteractor();
    ~PokeInteractor();

    // Override base methods
    virtual void before() override;
    virtual void process() override;
    virtual void find_potential_interactable() override;
    
    virtual bool get_should_start_hover() override;
    virtual bool get_should_activate() override;
    virtual bool get_should_end_activate() override;
    
    virtual void on_activate() override;
    virtual void on_end_activate() override;

    // Poke-specific methods
    bool get_is_poking() const;
    Vector3 get_poke_velocity() const;
    
    // Configuration getters/setters
    void set_poke_radius(float p_radius);
    float get_poke_radius() const;
    
    void set_poke_threshold(float p_threshold);
    float get_poke_threshold() const;
    
    void set_velocity_threshold(float p_threshold);
    float get_velocity_threshold() const;
    
    void set_max_poke_depth(float p_depth);
    float get_max_poke_depth() const;
    
    void set_interaction_search_radius(float p_radius);
    float get_interaction_search_radius() const;
};

#endif // POKE_INTERACTOR_H
