#include "replicated_physics_player_3d.h"
#include "scene/main/multiplayer_api.h"

void ReplicatedPhysicsPlayer3D::_ready() {
    ReplicatedRigidBody3D::_ready();
}

void ReplicatedPhysicsPlayer3D::_physics_process() {
    _consume_pending_input();
}

void ReplicatedPhysicsPlayer3D::_consume_pending_input() {
    if (!locally_controlled || !multiplayer || pending_input.is_empty()) { return; }

    // Apply input regardless if server or client, since this is using client-side prediction
    _apply_input(pending_input);

    // If this isn't the server, then we need to send the input to the server to perform the same logic
    if (!multiplayer->is_server()) {
        rpc_id(SERVER_ID, "_send_input_rpc", pending_input._serialize());
    }




    pending_input.reset();
}

void ReplicatedPhysicsPlayer3D::_apply_input(const PlayerInput &input) {
    Vector2 planar_force = Vector2(input.forward, input.left);

    if (!planar_force.is_zero_approx()) {
        Vector3 force = get_basis().get_column(0) * planar_force.y + get_basis().get_column(2) * planar_force.x;

        apply_central_force(force);
    }
}

// ----- Input methods exposed to GDScript -----
void ReplicatedPhysicsPlayer3D::add_forward_input(float axis_value) {
    pending_input.forward += axis_value;
}

void ReplicatedPhysicsPlayer3D::add_left_input(float axis_value) {
    pending_input.left += axis_value;
}

// ----- End input methods -----

// ----- RPC methods -----

void ReplicatedPhysicsPlayer3D::_send_input(const Dictionary &input) {
    print_line("Server received input from: ", multiplayer->get_remote_sender_id());
}

// ----- End RPC methods -----

// Bind methods for GDScript and other usage. This should be towards the end of the file
void ReplicatedPhysicsPlayer3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add_forward_input", "axis_value"), &ReplicatedPhysicsPlayer3D::add_forward_input);
    ClassDB::bind_method(D_METHOD("add_left_input", "axis_value"), &ReplicatedPhysicsPlayer3D::add_left_input);
    
    ClassDB::bind_method(D_METHOD("is_locally_controlled"), &ReplicatedPhysicsPlayer3D::is_locally_controlled);
    ClassDB::bind_method(D_METHOD("set_is_locally_controlled", "value"), &ReplicatedPhysicsPlayer3D::set_locally_controlled);

    // ----- RPC methods -----
    ClassDB::bind_method(D_METHOD("_send_input", "input"), &ReplicatedPhysicsPlayer3D::_send_input);
    // ----- End RPC methods -----
    
}

ReplicatedPhysicsPlayer3D::ReplicatedPhysicsPlayer3D() {
    // Empty constructor
}

ReplicatedPhysicsPlayer3D::~ReplicatedPhysicsPlayer3D() {
    // Empty destructor
}
