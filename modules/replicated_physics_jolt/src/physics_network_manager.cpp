#include "physics_network_manager.h"
#include "scene/main/multiplayer_api.h"

void PhysicsNetworkManager::_physics_process() {
    bool server = get_multiplayer().ptr()->is_server();

    physics_tick++;
}

void PhysicsNetworkManager::_notification(int p_what)
{
    // This line will stop the rest of this method from running in the editor.
    if (Engine::get_singleton()->is_editor_hint()) { return; }

    switch(p_what)
    {
        case NOTIFICATION_PHYSICS_PROCESS: {
            _physics_process();
            break;
        }
        default: {
            break;
        }
    }
}

void PhysicsNetworkManager::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_physics_tick"), &PhysicsNetworkManager::get_physics_tick);
    ClassDB::bind_method(D_METHOD("set_physics_tick", "tick"), &PhysicsNetworkManager::set_physics_tick);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_tick"), "set_physics_tick", "get_physics_tick");
}

PhysicsNetworkManager::PhysicsNetworkManager() {
    set_physics_process(true);
}

PhysicsNetworkManager::~PhysicsNetworkManager() {}
