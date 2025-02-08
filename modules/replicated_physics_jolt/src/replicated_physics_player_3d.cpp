#include "replicated_physics_player_3d.h"
#include "scene/main/multiplayer_api.h"

void ReplicatedPhysicsPlayer3D::_ready() {
	ReplicatedRigidBody3D::_ready();

	player_id = String(get_name()).to_int();
}

void ReplicatedPhysicsPlayer3D::_physics_process() {
	DEV_ASSERT(multiplayer);

	if (locally_controlled) {
		_handle_inputs();
		_apply_input(pending_input);

		if (!multiplayer->is_server()) {
			// ToDo: Check doing this logic only when there's input to apply

			PlayerState client_state;
			_fill_physics_state(client_state.physics_state);
			client_state.input = pending_input;
			client_state.sequence = input_sequence;

			state_buffer[state_buffer.get_next_index(input_sequence)] = client_state;

			// print_line("Client | Seq: ", client_state.sequence, " | state: ", client_state.physics_state._serialize());

			rpc_id(SERVER_ID, "_client_send_input_rpc", client_state._serialize());

			input_sequence++;
		}

		pending_input.reset(); // ToDo: Make sure having this here doesn't mess when more than 2 players (move it outside if statement if so)
	}

	if (!locally_controlled && multiplayer->is_server()) {
		_apply_input(pending_input);

		PlayerState server_state;
		_fill_physics_state(server_state.physics_state);
		server_state.sequence = last_sequence;

		// print_line("Server | Seq: ", server_state.sequence, " | state: ", server_state.physics_state._serialize());

		rpc_id(player_id, "_server_send_state_rpc", server_state._serialize());

		// ToDo: Check to see pending input in here, I'd expect it to always be empty

		// pending_input.reset();
	}
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
// ToDo: After testing, change this to use PlayerInput struct only, instead of PlayerState
void ReplicatedPhysicsPlayer3D::_client_send_input(const Dictionary &player_state) {
	DEV_ASSERT(multiplayer && multiplayer->is_server() && !locally_controlled);

    PlayerState client_state;
    client_state._deserialize(player_state);

	if (client_state.sequence > last_sequence) {
		// _apply_input(client_state.input);
		pending_input = client_state.input;
		last_sequence = client_state.sequence;
	}
}

/*
This RPC sends the physics state of the player from the server to the client to check for desync
*/
// @rpc("authority", "unreliable_ordered")
void ReplicatedPhysicsPlayer3D::_server_send_state(const Dictionary &in_server_state) {
	DEV_ASSERT(multiplayer && locally_controlled && !multiplayer->is_server());

	PlayerState server_state;
	server_state._deserialize(in_server_state);

	if (server_state.sequence <= last_sequence) { return; }

	last_sequence = server_state.sequence;

	PlayerState client_state = state_buffer[state_buffer.get_next_index(server_state.sequence)];

	print_line("Server | Seq: ", server_state.sequence, " | state: ", server_state.physics_state._serialize());
	print_line("Client | Seq: ", client_state.sequence, " | state: ", client_state.physics_state._serialize());
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
	ClassDB::bind_method(D_METHOD("_server_send_state", "in_server_state"), &ReplicatedPhysicsPlayer3D::_server_send_state);
	// ----- End RPC methods -----

	// ----- Getters/Setters -----

	// ----- End Getters/Setters -----
}

ReplicatedPhysicsPlayer3D::ReplicatedPhysicsPlayer3D()
{}

ReplicatedPhysicsPlayer3D::~ReplicatedPhysicsPlayer3D()
{}
