#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h"
#include "graphics/material.h"

enum {
	NONE = 0,
	FLOOR = 1 << 0,
	WALL = 1 << 1,
	PLAYER = 1 << 2,
	ENEMY = 1 << 3,
	// Both WALL and FLOOR are considered SCENARIO
	// using OR operator
	SCENARIO = WALL | FLOOR,
	ALL = 0xFF
};


class EntityMesh : public Entity {

	public:
	EntityMesh() {};  //Create empty entityMesh
	EntityMesh(Mesh* mesh, Material material); //Create entityMesh
	EntityMesh(Mesh* mesh, Shader* shader, Texture* texture, const std::string& name); //Create entityMesh
	EntityMesh(char* Smesh, char* shaderVs, char* shaderFs, char* Stexture, const std::string& name); //Create entityMesh
	~EntityMesh(); //desproy entityMesh

	// Attributes of the derived class  a
	Mesh* mesh = nullptr;
	Material material;
	bool isInstanced;
	int layer = NONE;

	std::vector<Matrix44> models_to_render;

	bool IsInstanciated();

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void setMaterial(Material material);
};