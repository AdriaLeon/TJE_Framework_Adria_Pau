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


void EntityCollider::testCollisionHightVelocity(Matrix44 model, Vector3 current_center, Vector3 next_center, std::vector<sCollisionData>& Collisions) {


	Vector3 colPoint, colNormal;

	// Calculate the direction and length of the movement
	Vector3 direction = (next_center - current_center).normalize();
	float distance = (next_center - current_center).length() + 0.05f;

	// Floor collisions
	if (mesh->testRayCollision(model, current_center, direction, colPoint, colNormal, distance)) {
		Collisions.push_back({ colPoint, colNormal.normalize() });
		//printf("on floor\n");
	}

}

void EntityCollider::getCollisionsHightVelocity(const Vector3& position, const Vector3& target_position, std::vector<sCollisionData>& Collisions)
{
	Vector3 current_center = position + Vector3(0.0f, player_height, 0.0f);
	Vector3 next_center = target_position + Vector3(0.0f, player_height, 0.0f);

	if (!isInstanced)
	{
		testCollisionHightVelocity(model, current_center, next_center, Collisions);
	}
	else
	{
		for (int i = 0; i < models.size(); ++i)
		{
			testCollisionHightVelocity(models[i], current_center, next_center, Collisions);
		}
	}
}

