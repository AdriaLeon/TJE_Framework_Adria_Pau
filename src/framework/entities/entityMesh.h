#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h"
#include "graphics/material.h"
#include "framework/animation.h"

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

struct StructAnimation{
	Animation* animation;
	std::string name;
};


class EntityMesh : public Entity {

	public:
	EntityMesh() {};  //Create empty entityMesh
	EntityMesh(Mesh* mesh, Material material); //Create entityMesh
	EntityMesh(Mesh* mesh, Shader* shader, Texture* texture, const std::string& name); //Create entityMesh
	EntityMesh(char* Smesh, char* shaderVs, char* shaderFs, char* Stexture, const std::string& name); //Create entityMesh
	~EntityMesh(); //desproy entityMesh

	// Attributes of the derived class
	Mesh* mesh = nullptr;
	Material material;
	bool isInstanced;
	int layer = NONE;
	std::string animation_in_use = "";

	std::vector <StructAnimation> Animations;

	std::vector<Matrix44> models_to_render;

	bool IsInstanciated();
	void addAnimation(Animation* animation, const std::string& name);
	bool Animation_appplied();

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void setMaterial(Material material);
};