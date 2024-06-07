#ifndef ENTITY_COLLIDER_H
#define ENTITY_COLLIDER_H

#include "entityMesh.h"
#include "game/world.h"


class EntityCollider : public EntityMesh {

public:

	float player_height = 5.0f;

	EntityCollider(Mesh* mesh, Material material) : EntityMesh(mesh, material) {};
	~EntityCollider() {};

	// New methods
	void testCollision(Matrix44 model, Vector3 character_center, std::vector<sCollisionData>& WallCollisions, std::vector<sCollisionData>& GroundCollisions);
	void getCollisions(const Vector3& target_position, std::vector<sCollisionData>& WallCollisions, std::vector<sCollisionData>& GroundCollisions);

	void testCollisionHightVelocity(Matrix44 model, Vector3 previous_center, Vector3 next_center, std::vector<sCollisionData>& Collisions);
	void getCollisionsHightVelocity(const Vector3& position, const Vector3& target_position, std::vector<sCollisionData>& Collisions);

	bool collisionAlreadyDetected(Vector3 colNormal, const std::vector<sCollisionData> collisions);
};

#endif // ENTITY_COLLIDER_H