#ifndef REPLICATED_PHYSICS_PLAYER_3D_H
#define REPLICATED_PHYSICS_PLAYER_3D_H

#include "replicated_rigid_body_3d.h"

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
    // ToDo: Implement player state
};

class ReplicatedPhysicsPlayer3D : public ReplicatedRigidBody3D {
    GDCLASS(ReplicatedPhysicsPlayer3D, ReplicatedRigidBody3D);

protected:
    PlayerInput pending_input;

    void _physics_process();
    void _consume_pending_input();

public:
    void add_forward_input(float axis_value);
    void add_left_input(float axis_value);
    
protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    ReplicatedPhysicsPlayer3D();
    ~ReplicatedPhysicsPlayer3D();
};





#endif // REPLICATED_PHYSICS_PLAYER_3D_H
