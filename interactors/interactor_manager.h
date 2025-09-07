#ifndef INTERACTOR_MANAGER_H
#define INTERACTOR_MANAGER_H

#include "interactor.h"
#include "scene/main/node.h"
#include "scene/main/scene_tree.h"
#include "scene/main/window.h"
#include "../surface3d.h"

class Interactor;
class Surface3D;

class InteractorManager : public Node {
    GDCLASS(InteractorManager, Node);

    static InteractorManager *singleton;

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    Vector<Interactor*> interactors;
    static void EnsureInteractorManagerExists();
    static InteractorManager *get_singleton();
    void process_interactors();
    void add_interactor(Interactor *p_interactor);
    void remove_interactor(Interactor *p_interactor);
    void setup_node();
    void cleanup_node();
    InteractorManager();
    ~InteractorManager();
};

#endif // INTERACTOR_MANAGER_H