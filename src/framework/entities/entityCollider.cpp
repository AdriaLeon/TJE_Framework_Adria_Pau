#include "entityCollider.h"
#include "graphics/mesh.h"
#include <cmath>

void EntityCollider::testCollision(Matrix44 model, Vector3 center, std::vector<sCollisionData>& WallCollisions, std::vector<sCollisionData>& GroundCollisions) {


	Vector3 colPoint, colNormal;

	// Floor collisions
	if (mesh->testRayCollision(model, center, Vector3(0.0, -1.0, 0.0) , colPoint, colNormal, player_height + 0.05f)) {//}, true)) {
		GroundCollisions.push_back({ colPoint, colNormal.normalize()});
		//printf("on floor\n");
	}

	// Wall collision
	float sphereRadius = 0.3f;

	center = center + Vector3(0.0f, -2.0f, 0.0f);

	for (int i = 0; i < 3; ++i) {
		if (mesh->testSphereCollision(model, center, sphereRadius, colPoint, colNormal)) {
			// Normalize the collision normal
			Vector3 normalizedColNormal = colNormal.normalize();

			// Check if this collision normal has already been detected
			if (!collisionAlreadyDetected(normalizedColNormal, WallCollisions)) {
				WallCollisions.push_back({ colPoint, normalizedColNormal });
			}
		}
		center = center + Vector3(0.0f, 0.6f, 0.0f);
	}

	// Generate directions using sin and cos
	std::vector<Vector3> directions;
	for (int i = 0; i < 4; ++i) {
		float angle = i * (M_PI / (2.0f)) + (M_PI / 4.0f); // Increment angle by 45 degrees (π/4 radians)
		directions.push_back(Vector3(cos(angle), 0, sin(angle)));
	}

	sphereRadius = 0.2f;
	float distance = sphereRadius; // Distance between the centers of touching spheres

	center = center - Vector3(0.0f, 0.3f, 0.0f);


	for (const auto& dir : directions) {
		Vector3 sphereCenter = center + dir * distance;
		if (mesh->testSphereCollision(model, sphereCenter, sphereRadius, colPoint, colNormal)) {
			// Normalize the collision normal
			Vector3 normalizedColNormal = colNormal.normalize();

			// Check if this collision normal has already been detected
			if (!collisionAlreadyDetected(normalizedColNormal, WallCollisions)) {
				WallCollisions.push_back({ colPoint, normalizedColNormal });
			}
		}
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

	// Fast collisions
	if (mesh->testRayCollision(model, current_center, direction, colPoint, colNormal, distance)) {
		Collisions.push_back({ colPoint - (direction * 0.05f) - Vector3(0.0f, player_height, 0.0f) , colNormal.normalize()});
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


bool EntityCollider::collisionAlreadyDetected(Vector3 colNormal, const std::vector<sCollisionData> collisions) {
	for (auto& collision : collisions) {
		if (collision.colNormal.x == colNormal.x && collision.colNormal.y == colNormal.y && collision.colNormal.z == colNormal.z) {
			return true;
		}
	}
	return false;
}

