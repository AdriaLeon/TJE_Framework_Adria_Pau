#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h"



class EntityMesh : public Entity {

	public:
	EntityMesh() {};
	EntityMesh(Mesh* mesh, const Texture* texture, const Shader* shader);
	~EntityMesh();

	// Attributes of the derived class  
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	Vector4 color;

	// Methods overwritten from base class
	void render();
	void update(float elapsed_time);
};


