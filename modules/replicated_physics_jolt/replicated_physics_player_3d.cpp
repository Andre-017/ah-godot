#include "replicated_physics_player_3d.h"

void ReplicatedPhysicsPlayer3D::_physics_process() {
    _consume_pending_input();
}

void ReplicatedPhysicsPlayer3D::_consume_pending_input() {
    Vector2 planar_force = Vector2(pending_input.forward, pending_input.left);

    if (!planar_force.is_zero_approx()) {
        Vector3 force = get_basis().get_column(0) * planar_force.y + get_basis().get_column(2) * planar_force.x;

        apply_central_force(force);
    }

    pending_input.reset();
}

// --- Input methods exposed to GDScript ---
void ReplicatedPhysicsPlayer3D::add_forward_input(float axis_value) {
    pending_input.forward += axis_value;
}

void ReplicatedPhysicsPlayer3D::add_left_input(float axis_value) {
    pending_input.left += axis_value;
}

// --- End input methods ---

void ReplicatedPhysicsPlayer3D::_notification(int p_what)
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

// Bind methods for GDScript and other usage. This should be towards the end of the file
void ReplicatedPhysicsPlayer3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add_forward_input", "axis_value"), &ReplicatedPhysicsPlayer3D::add_forward_input);
    ClassDB::bind_method(D_METHOD("add_left_input", "axis_value"), &ReplicatedPhysicsPlayer3D::add_left_input);
}

ReplicatedPhysicsPlayer3D::ReplicatedPhysicsPlayer3D() {
    // This must be set to true in order to receive the physics_process notification
    set_physics_process(true);
}

ReplicatedPhysicsPlayer3D::~ReplicatedPhysicsPlayer3D() {
    // Empty constructor
}
