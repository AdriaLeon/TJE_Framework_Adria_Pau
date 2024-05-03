#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h"
#include "graphics/material.h"



class EntityMesh : public Entity {

	public:
	EntityMesh() {};
	EntityMesh(Mesh* mesh, Texture* texture, Shader* shader, Vector4 color) {
		this->mesh = mesh;
		this->texture = texture;
		this->shader = shader;
		this->color = color;

	};
	//~EntityMesh(); TODO:

	// Attributes of the derived class  
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	Material* material = nullptr;
	Vector4 color;

	// Methods overwritten from base class
	void render(Matrix44 m);
	void update(float elapsed_time);
	void setMaterial(Material* material);
};


