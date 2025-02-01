#include "replicated_physics_player_3d.h"
#include "scene/main/multiplayer_api.h"

#include "physics_network_manager.h"

void ReplicatedPhysicsPlayer3D::_ready() {
	ReplicatedRigidBody3D::_ready();

	player_id = String(get_name()).to_int();
}

void ReplicatedPhysicsPlayer3D::_physics_process() {
	DEV_ASSERT(multiplayer);
	DEV_ASSERT(physics_manager);

	uint64_t physics_frame = Engine::get_singleton()->get_physics_frames();

	if (locally_controlled) {
		_handle_inputs(); // This will be called by GDScript, which will add to pending_input
		_consume_pending_input();
	}

	PhysicsState current_state;
	current_state.physics_frame = physics_frame;
	_fill_physics_state(current_state);

	const uint64_t physics_tick = physics_manager->get_physics_tick();

	if (multiplayer->is_server() && !locally_controlled) {
		state_buffer[state_buffer.get_next_index(physics_frame)] = current_state;
		print_line("Server | Physics tick: ", physics_tick);
		// print_line("Frame: ", physics_frame, " | Server state: ", state_buffer[state_buffer.get_next_index(physics_frame)]._serialize());
		// rpc_id(player_id, "_send_state_rpc", current_state._serialize());
	}

	if (!multiplayer->is_server() && locally_controlled) {
		state_buffer[state_buffer.get_next_index(physics_frame)] = current_state;
		print_line("Client | Physics tick: ", physics_tick);
		// print_line("Frame: ", physics_frame, " | Client state: ", state_buffer[state_buffer.get_next_index(physics_frame)]._serialize());
	}

	// if (locally_controlled ) {
	// 	_consume_pending_input();
	// }
}

void ReplicatedPhysicsPlayer3D::_consume_pending_input() {
	DEV_ASSERT(multiplayer);

	if (pending_input.is_empty()) { return; }

	// ToDo: Implement adjustment on client if needed

	if (locally_controlled) {
		uint64_t physics_frame = Engine::get_singleton()->get_physics_frames();

		// Apply input regardless if server or client, since this is using client-side prediction
		_apply_input(pending_input);

		// If this isn't the server, then we need to send the input to the server to perform the same logic
		if (!multiplayer->is_server()) {
			rpc_id(SERVER_ID, "_send_input_rpc", pending_input._serialize());
		}
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

void ReplicatedPhysicsPlayer3D::_fill_physics_state(PhysicsState &state) {
	state.position = get_global_position();
	state.rotation = get_global_transform().basis.get_quaternion();
	state.linear_velocity = get_linear_velocity();
	state.angular_velocity = get_angular_velocity();
}

// ----- Input methods exposed to GDScript -----
void ReplicatedPhysicsPlayer3D::add_forward_input(float axis_value) {
	pending_input.forward += axis_value;
}

void ReplicatedPhysicsPlayer3D::add_left_input(float axis_value) {
	pending_input.left += axis_value;
}

// ----- End input methods -----

/*
----- RPC methods -----
These methods are called from GDScript to perform the logic of the RPC. The rpc's themselves are defined in GDScript
*/

/*
This RPC is called from the client to the server to send input. The server will then apply the input to the player
*/
// @rpc("any_peer", "unreliable_ordered")
void ReplicatedPhysicsPlayer3D::_send_input(const Dictionary &input) {
	DEV_ASSERT(multiplayer);

	if (!multiplayer->is_server() || locally_controlled) { return; }

    PlayerInput client_input;
    client_input._deserialize(input);

    _apply_input(client_input);
}

/*
This RPC sends the physics state of the player from the server to the client for sync purposes
*/
// @rpc("authority", "unreliable_ordered")
void ReplicatedPhysicsPlayer3D::_send_state(const Dictionary &state) {
	DEV_ASSERT(multiplayer);

	if (!locally_controlled || multiplayer->is_server()) { return; }

	PhysicsState server_state;
	server_state._deserialize(state);

	// print_line("Server state: ", server_state._serialize());

	// PhysicsState client_state = state_buffer.get(server_state.physics_frame);
	PhysicsState client_state = state_buffer[state_buffer.get_next_index(server_state.physics_frame)];

	// print_line("Client state: ", client_state._serialize());

	bool sync = !client_state.is_approx_equal(server_state);
	// print_line("Sync: ", sync);
}

// ----- End RPC methods -----

// Bind methods for GDScript and other usage. This should be towards the end of the file
void ReplicatedPhysicsPlayer3D::_bind_methods() {
	GDVIRTUAL_BIND(_handle_inputs);

	ClassDB::bind_method(D_METHOD("add_forward_input", "axis_value"), &ReplicatedPhysicsPlayer3D::add_forward_input);
	ClassDB::bind_method(D_METHOD("add_left_input", "axis_value"), &ReplicatedPhysicsPlayer3D::add_left_input);

	ClassDB::bind_method(D_METHOD("is_locally_controlled"), &ReplicatedPhysicsPlayer3D::is_locally_controlled);
	ClassDB::bind_method(D_METHOD("set_is_locally_controlled", "value"), &ReplicatedPhysicsPlayer3D::set_locally_controlled);

	/*
	----- RPC methods -----
	The below method bindings are for RPC methods. Since these rpc's need to be defined in GDScript,
	we'll define them there and then call these methods to perform the logic
	*/
	ClassDB::bind_method(D_METHOD("_send_input", "input"), &ReplicatedPhysicsPlayer3D::_send_input);
	ClassDB::bind_method(D_METHOD("_send_state", "state"), &ReplicatedPhysicsPlayer3D::_send_state);
	// ----- End RPC methods -----

	// ----- Getters/Setters -----
	ClassDB::bind_method(D_METHOD("set_physics_manager", "manager"), &ReplicatedPhysicsPlayer3D::set_physics_manager);
	// ----- End Getters/Setters -----
}

// ReplicatedPhysicsPlayer3D::ReplicatedPhysicsPlayer3D(): 
// 	state_buffer(PHYSICS_STATE_BUFFER_SIZE),
// 	input_history(PHYSICS_STATE_BUFFER_SIZE)
// {}

ReplicatedPhysicsPlayer3D::ReplicatedPhysicsPlayer3D()
{}

ReplicatedPhysicsPlayer3D::~ReplicatedPhysicsPlayer3D()
{}
