#ifndef POKE_INTERACTABLE_H
#define POKE_INTERACTABLE_H
 
#include "interactable.h"
#include "../interactors/poke_interactor.h"
#include "../surface3d.h"
 
class PokeInteractable : public Interactable {
    GDCLASS(PokeInteractable, Interactable)
 
protected:
    struct PokeState {
        Vector3 contact_point;
        Vector3 surface_normal;
        float poke_depth;
        float max_depth_reached;
        bool is_active;
        
        PokeState() : poke_depth(0.0f), max_depth_reached(0.0f), is_active(false) {}
    };
 
    static void _bind_methods();
 
private:
    // Poke-specific properties
    float _poke_response_threshold = 0.01f;  // Minimum poke depth to respond
    float _max_poke_depth = 0.05f;  // Maximum allowed poke depth
    bool _requires_surface = true;  // Whether this interactable requires a Surface3D
    bool _consume_poke = false;  // Whether to prevent poke from affecting other interactables
    
    // Current poke state
    Vector<PokeInteractor*> _poking_interactors;
    HashMap<PokeInteractor*, PokeState> _poke_states;  // Maps interactor to poke data
    
    // Surface integration
    Surface3D* _surface = nullptr;
    bool _auto_find_surface = true;
 
public:
    PokeInteractable();
    virtual ~PokeInteractable();
 
    // Override base interactable methods
    virtual bool can_interact_with(Interactor* interactor) override;
    virtual void on_activate(Interactor* interactor) override;
    virtual void on_end_activate(Interactor* interactor) override;
    virtual void on_hover_start(Interactor* interactor) override;
    virtual void on_hover_end(Interactor* interactor) override;
 
    // Poke-specific interaction methods
    virtual bool can_be_poked_by(PokeInteractor* poke_interactor);
    virtual void on_poke_start(PokeInteractor* poke_interactor, const Vector3& contact_point, const Vector3& surface_normal);
    virtual void on_poke_update(PokeInteractor* poke_interactor, float poke_depth, const Vector3& contact_point);
    virtual void on_poke_end(PokeInteractor* poke_interactor, float max_depth_reached);
 
    // Poke state queries
    bool is_being_poked() const;
    bool is_being_poked_by(PokeInteractor* poke_interactor) const;
    Vector<PokeInteractor*> get_poking_interactors() const;
    float get_total_poke_depth() const;
    float get_max_poke_depth_by(PokeInteractor* poke_interactor) const;
    Vector3 get_poke_contact_point(PokeInteractor* poke_interactor) const;
 
    // Configuration
    void set_poke_response_threshold(float p_threshold);
    float get_poke_response_threshold() const;
    
    void set_max_poke_depth(float p_depth);
    float get_max_poke_depth() const;

    void set_consume_poke(bool p_consume);
    bool get_consume_poke() const;

    PokeState* get_poke_state(PokeInteractor* poke_interactor) const;

    Surface3D* get_surface() const;
    void set_surface(Surface3D* p_surface);
 
protected:
    virtual bool _validate_poke_interaction(PokeInteractor* poke_interactor);
    void _update_poke_state(PokeInteractor* poke_interactor);
    float _calculate_poke_depth(PokeInteractor* poke_interactor);
    bool _check_surface_penetration(PokeInteractor* poke_interactor);
    Vector3 _get_contact_point(PokeInteractor* poke_interactor);
    Vector3 _get_surface_normal(PokeInteractor* poke_interactor);
};
 
#endif // POKE_INTERACTABLE_H