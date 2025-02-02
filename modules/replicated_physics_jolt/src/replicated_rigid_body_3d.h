#ifndef REPLICATED_RIGID_BODY_3D_H
#define REPLICATED_RIGID_BODY_3D_H

#include "scene/3d/physics/rigid_body_3d.h"

struct PhysicsState {
    PhysicsState() {
        reset();
    }

    // uint64_t physics_frame;
    Vector3 position;
    Quaternion rotation;
    Vector3 linear_velocity;
    Vector3 angular_velocity;

    void reset() {
        // physics_frame = 0;
        position = Vector3();
        rotation = Quaternion();
        linear_velocity = Vector3();
        angular_velocity = Vector3();
    }

    bool is_approx_equal(const PhysicsState &p_other) const {
        if (!position.is_equal_approx(p_other.position)) { return false; }
        if (!linear_velocity.is_equal_approx(p_other.linear_velocity)) { return false; }
        if (!angular_velocity.is_equal_approx(p_other.angular_velocity)) { return false; }
        if (!rotation.is_equal_approx(p_other.rotation)) { return false; }

        return true;
    }

    Dictionary _serialize() const {
        Dictionary dict;

        // dict["physics_frame"] = physics_frame;
        dict["position"] = position;
        dict["rotation"] = rotation; // ToDo: Quantize this

        if (!linear_velocity.is_zero_approx()) {
            dict["linear_velocity"] = linear_velocity;
        }

        if (!angular_velocity.is_zero_approx()) {
            dict["angular_velocity"] = angular_velocity;
        }

        return dict;
    }

    void _deserialize(const Dictionary &dict) {
        // physics_frame = dict.get("physics_frame", 0);
        position = dict.get("position", Vector3());
        rotation = dict.get("rotation", Quaternion());
        linear_velocity = dict.get("linear_velocity", Vector3());
        angular_velocity = dict.get("angular_velocity", Vector3());
    }
};

class ReplicatedRigidBody3D : public RigidBody3D {
    GDCLASS(ReplicatedRigidBody3D, RigidBody3D);

protected:
    MultiplayerAPI *multiplayer = nullptr;

protected:
    virtual void _ready();
    virtual void _physics_process() {};
    
    void _notification(int p_what);

protected:
    static void _bind_methods();

public:
    ReplicatedRigidBody3D();
};





#endif // REPLICATED_RIGID_BODY_3D_H
