#ifndef PHYSICS_NETWORK_MANAGER_H
#define PHYSICS_NETWORK_MANAGER_H

#include "scene/main/node.h"

class PhysicsNetworkManager : public Node {
    GDCLASS(PhysicsNetworkManager, Node);

private:
    uint64_t physics_tick = 0;

public:
    uint64_t get_physics_tick() const { return physics_tick; }
    void set_physics_tick(uint64_t tick) { physics_tick = tick; }

    // static PhysicsNetworkManager* get_singleton() { return singleton; }

    void increment_physics_tick() { physics_tick++; }

protected:
    virtual void _physics_process();   
    void _notification(int p_what);

    static void _bind_methods();

    // static PhysicsNetworkManager* singleton;

public:
    PhysicsNetworkManager();
    ~PhysicsNetworkManager();
};


#endif // PHYSICS_NETWORK_MANAGER_H
