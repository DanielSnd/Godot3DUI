#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "scene/3d/node_3d.h"
#include "../interactors/interactor.h"

class Interactor;
class InteractorManager;

class Interactable : public Node3D {
    GDCLASS(Interactable, Node3D)

protected:
    static void _bind_methods();
    void _notification(int p_what);

private:
    bool _is_enabled = true;
    bool _is_hovered = false;
    bool _is_activated = false;
    bool _can_be_interacted = true;
    
    // Currently interacting interactors
    Vector<Interactor*> _hovering_interactors;
    Vector<Interactor*> _activating_interactors;

public:
    Interactable();
    virtual ~Interactable();

    // Core interaction methods - override these in derived classes
    virtual bool can_interact_with(Interactor* interactor);
    virtual void on_hover_start(Interactor* interactor);
    virtual void on_hover_end(Interactor* interactor);
    virtual void on_activate(Interactor* interactor);
    virtual void on_end_activate(Interactor* interactor);
    virtual void on_interact(Interactor* interactor);
    virtual void on_end_interact(Interactor* interactor);

    // State queries
    bool is_enabled() const;
    void set_enabled(bool p_enabled);
    
    bool is_hovered() const;
    bool is_activated() const;
    bool can_be_interacted() const;
    void set_can_be_interacted(bool p_can_interact);

    // Interactor management
    bool is_hovered_by(Interactor* interactor) const;
    bool is_activated_by(Interactor* interactor) const;
    Vector<Interactor*> get_hovering_interactors() const;
    Vector<Interactor*> get_activating_interactors() const;

protected:
    // Internal state management
    void _add_hovering_interactor(Interactor* interactor);
    void _remove_hovering_interactor(Interactor* interactor);
    void _add_activating_interactor(Interactor* interactor);
    void _remove_activating_interactor(Interactor* interactor);

    // Override this to provide custom interaction validation
    virtual bool _validate_interaction(Interactor* interactor);
};

#endif // INTERACTABLE_H
