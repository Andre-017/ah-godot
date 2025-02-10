/* register_types.cpp */

#include "register_types.h"

#include "core/object/class_db.h"
#include "src/replicated_rigid_body_3d.h"
#include "src/replicated_physics_player_3d.h"
#include "src/circular_buffer.h"
#include "src/circular_queue.h"
#include "src/physics_network_manager.h"

void initialize_replicated_physics_jolt_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<ReplicatedRigidBody3D>();
	ClassDB::register_class<ReplicatedPhysicsPlayer3D>();
	ClassDB::register_class<PhysicsNetworkManager>();
}

void uninitialize_replicated_physics_jolt_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
   // Nothing to do here in this example.
}
