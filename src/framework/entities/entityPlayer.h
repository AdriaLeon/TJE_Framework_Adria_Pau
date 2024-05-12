#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h";
#include "graphics/material.h"

class Camera;

class EntityPlayer : public Entity {

public:
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	bool onFloor;
	float walkSpeed;

	EntityPlayer() {};  //Create empty entityPlayer
	EntityPlayer(Mesh* mesh, Material* material); //Create entityMesh

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void setMaterial(Material* material);
};