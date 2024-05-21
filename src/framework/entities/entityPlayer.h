#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h";
#include "graphics/material.h"

enum {
	NONE = 0,
	FLOOR = 1 << 0,
	WALL = 1 << 1,
	PLAYER = 1 << 2,
	// Both WALL and FLOOR are considered SCENARIO
	// using OR operator
	SCENARIO = WALL | FLOOR,
	ALL = 0xFF
};

class Camera;

class EntityPlayer : public Entity {

public:
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	bool onFloor;
	float walkSpeed;
	Vector3 velocity;
	float height;

	//Parameters to perform collisions
	int layer = NONE;
	Vector3 col_point;
	Vector3 col_normal;

	EntityPlayer() {};  //Create empty entityPlayer
	EntityPlayer(Mesh* mesh, Material* material); //Create entityMesh
	bool is_colliding();

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void setMaterial(Material* material);
};