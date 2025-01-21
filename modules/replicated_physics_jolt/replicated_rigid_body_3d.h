#ifndef REPLICATED_RIGID_BODY_3D_H
#define REPLICATED_RIGID_BODY_3D_H

#include "scene/3d/physics/rigid_body_3d.h"

class ReplicatedRigidBody3D : public RigidBody3D {
    GDCLASS(ReplicatedRigidBody3D, RigidBody3D);

protected:
    static void _bind_methods();

public:

    ReplicatedRigidBody3D();
};





#endif // REPLICATED_RIGID_BODY_3D_H
