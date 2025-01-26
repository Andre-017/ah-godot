#ifndef REPLICATED_PHYSICS_PLAYER_3D_H
#define REPLICATED_PHYSICS_PLAYER_3D_H

#include "replicated_rigid_body_3d.h"

// Default id of the server in Godot multiplayer
const int SERVER_ID = 1;

struct PlayerInput {
    PlayerInput() {
        reset();
    }

    float forward;
    float left;

    void reset() {
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

        if (!Math::is_zero_approx(Math::abs(forward))) {
            dict["forward"] = forward;
        }

        if (!Math::is_zero_approx(Math::abs(left))) {
            dict["left"] = left;
        }

        return dict;
    }

    void _deserialize(const Dictionary &dict) {
        if (dict.has("forward")) {
            forward = static_cast<float>(dict["forward"]);
        } else {
            forward = 0.0f;
        }

        if (dict.has("left")) {
            left = static_cast<float>(dict["left"]);
        } else {
            left = 0.0f;
        }
    }
   
};

struct PlayerState {
    // ToDo: Implement player state. Should this go in the ReplicatedRighidBody3D class?
};

class ReplicatedPhysicsPlayer3D : public ReplicatedRigidBody3D {
    GDCLASS(ReplicatedPhysicsPlayer3D, ReplicatedRigidBody3D);

protected:
    PlayerInput pending_input;

    void _ready() override;
    void _physics_process() override;
    void _consume_pending_input();
    void _apply_input(const PlayerInput &input);

    bool locally_controlled = false;

public:
    // Set locally_controlled from GDScript _ready() method. Doing it here in C++ doesn't guarantee it's set before _ready() is called in GDScript
    bool is_locally_controlled() const { return locally_controlled; };
    void set_locally_controlled(bool value) { locally_controlled = value; };

public:
    // ----- Input methods -----
    void add_forward_input(float axis_value);
    void add_left_input(float axis_value);

    // ----- End input methods -----

protected:
    // ----- RPC methods -----
    void _send_input(const Dictionary &input);

    // ----- End RPC methods -----
    
protected:
    static void _bind_methods();

public:
    ReplicatedPhysicsPlayer3D();
    ~ReplicatedPhysicsPlayer3D();
};





#endif // REPLICATED_PHYSICS_PLAYER_3D_H
