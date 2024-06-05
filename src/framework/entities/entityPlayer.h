#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h";
#include "graphics/material.h"
#include "framework/audio.h"
#include "entityMesh.h"


class Camera;

class EntityPlayer : public EntityMesh {

public:
	bool onFloor;
	float walkSpeed;
	Vector3 velocity;
	bool is_colliding = false;
	bool is_jumping;
	float jumpSpeed;
	float gravity = 0;
	bool ground_pound = false;
	bool is_dashing = false;
	float dash_timer = 0.0;
	float time_for_groundpound = 0.0;
	bool move_xz;
	float dash_cooldown;
	bool sprinting;

	//Parameters to perform collisions
	float height;

	//Parameters of coyote time
	float coyoteTime;
	float timeSinceGrounded;

	//Parameters for handling sound effects
	HCHANNEL groundPoundChannel;
	bool hasLanded;

	//Channels stored
	EntityPlayer() {};  //Create empty entityPlayer
	EntityPlayer(Mesh* mesh, Material material); //Create entityPlayer

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void setMaterial(Material material);
	void check_collision(Vector3 next_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& GroundCollisions);
	void check_collisionHightVelocity(Vector3 position, Vector3 next_position, std::vector<sCollisionData>& Collisions);
	void handle_collisions(std::vector<sCollisionData> FastCollisions, std::vector<sCollisionData> WallsCollisions, std::vector<sCollisionData> GroundCollisions, Vector3& position, float elapsed_time, Vector3& final_vel);
	void handle_inputs(Vector3& move_dir, Matrix44 mYaw, Vector3& position);
};