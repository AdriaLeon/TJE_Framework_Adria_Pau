#include "entityCollider.h"
#include "graphics/mesh.h"

void EntityCollider::testCollision(Matrix44 model, Vector3 center, std::vector<sCollisionData>& WallCollisions, std::vector<sCollisionData>& GroundCollisions) {


	Vector3 colPoint, colNormal;

	// Floor collisions
	if (mesh->testRayCollision(model, center, Vector3(0.0, -1.0, 0.0) , colPoint, colNormal, player_height + 0.1f)) {//}, true)) {
		GroundCollisions.push_back({ colPoint, colNormal.normalize()});
		//printf("on floor\n");
	}

	// Wall collision
	float sphereRadius = 0.05f;

	if (mesh->testSphereCollision(model, center, sphereRadius, colPoint, colNormal)) {
		WallCollisions.push_back({ colPoint, colNormal.normalize()});
		//printf("on wall\n");
	}
}

void EntityCollider::getCollisions(const Vector3& target_position, std::vector<sCollisionData>& WallCollisions, std::vector<sCollisionData>& GroundCollisions)
{
	Vector3 center = target_position + Vector3(0.0f, player_height, 0.0f);

	if (!isInstanced)
	{
		testCollision(model, center, WallCollisions, GroundCollisions);
	}
	else
	{
		for (int i = 0; i < models.size(); ++i)
		{
			testCollision(models[i], center, WallCollisions, GroundCollisions);
		}
	}
}
