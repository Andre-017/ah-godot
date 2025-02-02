#include "replicated_rigid_body_3d.h"
#include "scene/main/multiplayer_api.h"

void ReplicatedRigidBody3D::_ready() {
    multiplayer = get_multiplayer().ptr();
    
    // This must be set to true in order to receive the physics_process notification
    set_physics_process(true);
}

void ReplicatedRigidBody3D::_notification(int p_what)
{
    // This line will stop the rest of this method from running in the editor.
    if (Engine::get_singleton()->is_editor_hint()) { return; }

    switch(p_what)
    {
        case NOTIFICATION_READY: {
            _ready();
            break;
        }
        case NOTIFICATION_PHYSICS_PROCESS: {
            _physics_process();
            break;
        }
        default: {
            break;
        }
    }
}

void ReplicatedRigidBody3D::_bind_methods() {

}

ReplicatedRigidBody3D::ReplicatedRigidBody3D() {
}
