#ifndef INTERACTOR_H
#define INTERACTOR_H

#include "scene/3d/node_3d.h"
#include "interactor_manager.h"
#include "../interactables/interactable.h"

class InteractionManager;
class Interactable;

class Interactor : public Node3D {
    GDCLASS(Interactor, Node3D)

protected:
    static void _bind_methods();
    void _notification(int p_what);
    
    Variant potential_interactable;
    Variant current_interactable;
    bool has_potential_interactable = false;
    bool has_interactable = false;
    bool has_activated_interactable = false;
    bool is_part_of_group = false;

public:
    Interactor();
    ~Interactor();

    enum InteractorEvent {
        INTERACTOR_EVENT_CANCEL,
        INTERACTOR_EVENT_ENABLE,
        INTERACTOR_EVENT_DISABLE,
        INTERACTOR_EVENT_HOVER_START,
        INTERACTOR_EVENT_HOVER_END,
        INTERACTOR_EVENT_ACTIVATE,
        INTERACTOR_EVENT_END_ACTIVATE,
        INTERACTOR_EVENT_INTERACT,
        INTERACTOR_EVENT_END_INTERACT,
        INTERACTOR_EVENT_MOVEMENT
    };

    // Main process methods
    virtual void before();
    virtual void process();
    virtual void after();

    // Interactable discovery
    virtual void find_potential_interactable();
    virtual bool get_has_potential_interactable() const;
    virtual Variant get_potential_interactable() const;

    // Hover system
    virtual bool get_should_start_hover();
    virtual void on_hover();
    
    virtual bool get_should_end_hover();
    virtual void on_hover_end();

    // Activation system
    virtual bool get_should_activate();
    virtual void on_activate();
    
    virtual bool get_should_end_activate();
    virtual void on_end_activate();

    // State queries
    virtual bool get_has_interactable() const;
    virtual bool get_has_activated_interactable() const;

    bool get_is_part_of_group() const;
};

VARIANT_ENUM_CAST(Interactor::InteractorEvent);

#endif // INTERACTOR_H
