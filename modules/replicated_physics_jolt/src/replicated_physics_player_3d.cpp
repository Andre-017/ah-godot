#include "replicated_physics_player_3d.h"
#include "scene/main/multiplayer_api.h"

#include "physics_network_manager.h"

void ReplicatedPhysicsPlayer3D::_ready() {
	ReplicatedRigidBody3D::_ready();

	player_id = String(get_name()).to_int();
}

void ReplicatedPhysicsPlayer3D::_physics_process() {
	DEV_ASSERT(multiplayer);

	if (locally_controlled) {
		_handle_inputs();
		_consume_pending_input();
	}

	if (!multiplayer->is_server() && locally_controlled) {
		PlayerState client_state;
		_fill_physics_state(client_state.physics_state);
		client_state.input = pending_input;
		client_state.sequence = input_sequence;

		state_buffer[state_buffer.get_next_index(input_sequence)] = client_state;

		rpc_id(SERVER_ID, "_client_send_input_rpc", client_state._serialize());

		input_sequence++;
	}

	pending_input.reset();
}

void ReplicatedPhysicsPlayer3D::_consume_pending_input() {
	DEV_ASSERT(multiplayer);

	if (pending_input.is_empty()) { return; }

	// ToDo: Implement adjustment on client if needed

	if (locally_controlled) {
		// uint64_t physics_frame = Engine::get_singleton()->get_physics_frames();

		// Apply input regardless if server or client, since this is using client-side prediction
		_apply_input(pending_input);

		// // If this isn't the server, then we need to send the input to the server to perform the same logic
		// if (!multiplayer->is_server()) {
		// 	rpc_id(SERVER_ID, "_client_send_input_rpc", pending_input._serialize());
		// }
	}

	// pending_input.reset();
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
void ReplicatedPhysicsPlayer3D::_client_send_input(const Dictionary &player_state) {
	DEV_ASSERT(multiplayer && multiplayer->is_server() && !locally_controlled);

    PlayerState client_state;
    client_state._deserialize(player_state);

	if (client_state.sequence > last_sequence) {
		_apply_input(client_state.input);
		last_sequence = client_state.sequence;
	}
}

/*
This RPC sends the physics state of the player from the client to the server to check for desync
*/
// @rpc("any_peer", "unreliable_ordered")
void ReplicatedPhysicsPlayer3D::_client_send_state(const Dictionary &in_client_state) {
	DEV_ASSERT(multiplayer && !locally_controlled && multiplayer->is_server());

	// ToDo: Get the tick offset between server and client, using something like a "first_frame" variable

	// PhysicsState client_state;
	// client_state._deserialize(in_client_state);

	// if (first_frame == -1) {
	// 	print_line("Server | client frame: ", client_state.physics_frame);
	// 	print_line("Server current frame: ", Engine::get_singleton()->get_physics_frames());
	// 	first_frame = client_state.physics_frame;
	// 	tick_offset = Engine::get_singleton()->get_physics_frames() - client_state.physics_frame;
	// 	print_line("Tick offset: ", tick_offset);
	// }

	// PhysicsState server_state = state_buffer[state_buffer.get_next_index(client_state.physics_frame + tick_offset)];

	// print_line("Client state: ", client_state._serialize());
	// print_line("Server state: ", server_state._serialize());

	// print_line("Server | current frame: ", Engine::get_singleton()->get_physics_frames());
	// print_line("Server | client state: ", client_state._serialize());

	// PhysicsState client_state = state_buffer.get(server_state.physics_frame);
	// PhysicsState client_state = state_buffer[state_buffer.get_next_index(server_state.physics_frame)];

	// print_line("Client state: ", client_state._serialize());

	// bool sync = !client_state.is_approx_equal(server_state);
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
	ClassDB::bind_method(D_METHOD("_client_send_input", "player_state"), &ReplicatedPhysicsPlayer3D::_client_send_input);
	ClassDB::bind_method(D_METHOD("_client_send_state", "in_client_state"), &ReplicatedPhysicsPlayer3D::_client_send_state);
	// ----- End RPC methods -----

	// ----- Getters/Setters -----
	// ClassDB::bind_method(D_METHOD("set_physics_manager", "manager"), &ReplicatedPhysicsPlayer3D::set_physics_manager);
	// ----- End Getters/Setters -----
}

ReplicatedPhysicsPlayer3D::ReplicatedPhysicsPlayer3D()
{}

ReplicatedPhysicsPlayer3D::~ReplicatedPhysicsPlayer3D()
{}
