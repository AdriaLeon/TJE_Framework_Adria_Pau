#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h"
#include "graphics/material.h"



class EntityMesh : public Entity {

	public:
	EntityMesh() {};
	EntityMesh(Mesh* mesh, const Material* material, const std::string& name = "");
	~EntityMesh();

	// Attributes of the derived class  
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	Vector4 color;

	Material material;

	// Methods overwritten from base class
	void render();
	void update(float elapsed_time);
};


