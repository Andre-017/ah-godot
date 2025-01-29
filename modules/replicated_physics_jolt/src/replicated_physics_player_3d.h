#ifndef REPLICATED_PHYSICS_PLAYER_3D_H
#define REPLICATED_PHYSICS_PLAYER_3D_H

#include "replicated_rigid_body_3d.h"
#include "circular_buffer.h"

// Default id of the server in Godot multiplayer
const int SERVER_ID = 1;

// Number of physics frames to hold in buffer
const int PHYSICS_STATE_BUFFER_SIZE = 200;

struct PlayerInput {
    PlayerInput() {
        reset();
    }

    uint64_t physics_frame;

    float forward;
    float left;

    void reset() {
        physics_frame = 0;
        forward = 0.0f;
        left = 0.0f;
    }

    bool is_empty() const {
        if (!Math::is_zero_approx(forward)) { return false; }
        if (!Math::is_zero_approx(left)) { return false; }

        return true;
    }

    Dictionary _serialize() const {
        Dictionary dict;

        dict["physics_frame"] = physics_frame;

        if (!Math::is_zero_approx(Math::abs(forward))) {
            dict["forward"] = forward;
        }

        if (!Math::is_zero_approx(Math::abs(left))) {
            dict["left"] = left;
        }

        return dict;
    }

    void _deserialize(const Dictionary &dict) {
        physics_frame = dict.get("physics_frame", 0);
        forward = dict.get("forward", 0.0f);
        left = dict.get("left", 0.0f);
    }
};

// struct PlayerState {
//     PlayerState() {
//         reset();
//     }
    
//     uint64_t physics_frame;
//     PlayerInput input;
//     PhysicsState physics_state;

//     void reset() {
//         physics_frame = 0;
//         input.reset();
//         physics_state.reset();
//     }

//     Dictionary _serialize() const {
//         Dictionary dict;

//         dict["physics_frame"] = physics_frame;
//         dict["input"] = input._serialize();
//         dict["physics_state"] = physics_state._serialize();
//     }

//     void _deserialize(const Dictionary &dict) {
//         physics_frame = dict.get("physics_frame", 0);
//         input._deserialize(dict.get("input", Dictionary()));
//         physics_state._deserialize(dict.get("physics_state", Dictionary()));
//     }
// };

class ReplicatedPhysicsPlayer3D : public ReplicatedRigidBody3D {
    GDCLASS(ReplicatedPhysicsPlayer3D, ReplicatedRigidBody3D);

protected:
    uint64_t physics_tick = 0; // Replicated physics_tick. This needs to be set from the PhysicsNetworkManager on _ready

    int player_id; // Unique id for this player

    PlayerInput pending_input;
    // CircularBuffer<PlayerInput> input_history;
    CircularBuffer<PhysicsState> state_buffer = CircularBuffer<PhysicsState>(PHYSICS_STATE_BUFFER_SIZE);

    void _ready() override;
    void _physics_process() override;
    void _consume_pending_input();
    void _apply_input(const PlayerInput &input);
    void _fill_physics_state(PhysicsState &state);

    bool locally_controlled = false;

public:
    // Set locally_controlled from GDScript _ready() method. Doing it here in C++ doesn't guarantee it's set before _ready() is called in GDScript
    bool is_locally_controlled() const { return locally_controlled; };
    void set_locally_controlled(bool value) { locally_controlled = value; };

public:
    /*
    This function will be overriden by a GDScript. We use this in order to properly handle input order
    between GDScript and C++ _physics_process calls
    */
    virtual void _handle_inputs() { GDVIRTUAL_CALL(_handle_inputs); };

    // ----- Input methods -----
    void add_forward_input(float axis_value);
    void add_left_input(float axis_value);

    // ----- End input methods -----

protected:
    // ----- RPC methods -----
    void _send_input(const Dictionary &input);
    void _send_state(const Dictionary &state);

    // ----- End RPC methods -----

public:
    // ----- Getters/Setters -----
    void set_physics_tick(uint64_t value) { physics_tick = value; }
    // ----- End Getters/Setters -----
    
protected:
    static void _bind_methods();

    GDVIRTUAL0(_handle_inputs)

public:
    ReplicatedPhysicsPlayer3D();
    ~ReplicatedPhysicsPlayer3D();
};





#endif // REPLICATED_PHYSICS_PLAYER_3D_H
