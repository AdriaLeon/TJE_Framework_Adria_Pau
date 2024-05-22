#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h";
#include "graphics/material.h"
#include "entityMesh.h"


class Camera;

class EntityPlayer : public EntityMesh {

public:
	bool onFloor;
	float walkSpeed;
	Vector3 velocity;
	bool is_colliding = false;

	//Parameters to perform collisions
	float height;

	EntityPlayer() {};  //Create empty entityPlayer
	EntityPlayer(Mesh* mesh, Material material); //Create entityPlayer

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void setMaterial(Material material);
	bool check_collision(Vector3 next_position);
};