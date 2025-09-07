#include "interactor_manager.h"

InteractorManager *InteractorManager::singleton = nullptr;

void InteractorManager::_bind_methods() {
}

InteractorManager *InteractorManager::get_singleton() {
    return singleton;
}

void InteractorManager::_notification(int p_what) {
    if (p_what == NOTIFICATION_ENTER_TREE) {
        singleton = this;
    }
    else if (p_what == NOTIFICATION_PROCESS) {
        process_interactors();
    }
}

void InteractorManager::EnsureInteractorManagerExists() {
    if (singleton == nullptr) {
        singleton = memnew(InteractorManager);
        singleton->setup_node();
    }
}

void InteractorManager::setup_node() {
    if (SceneTree::get_singleton() != nullptr) {
        set_name("InteractorManager");
        SceneTree::get_singleton()->get_root()->connect(SceneStringName(tree_exiting), callable_mp(this, &InteractorManager::cleanup_node));
        SceneTree::get_singleton()->get_root()->call_deferred("add_child", this);
    }
}

void InteractorManager::cleanup_node() {
    if (is_inside_tree()) {
        queue_free();
    }
}

void InteractorManager::add_interactor(Interactor *p_interactor) {
    interactors.push_back(p_interactor);
}

void InteractorManager::remove_interactor(Interactor *p_interactor) {
    for (int i = 0; i < interactors.size(); i++) {
        if (interactors[i] == p_interactor) {
            interactors.remove_at(i);
            break;
        }
    }
}

void InteractorManager::process_interactors() {
    for (int i = interactors.size() - 1; i >= 0; i--) {
        if (interactors[i] == nullptr || interactors[i]->get_is_part_of_group()) {
            interactors.remove_at(i);
        }
    }
    for (int i = 0; i < interactors.size(); i++) {
        interactors[i]->process();
    }
}

InteractorManager::InteractorManager() {
    // Constructor
}

InteractorManager::~InteractorManager() {
    // Destructor
}