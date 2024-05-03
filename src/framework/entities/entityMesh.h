#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h"
#include "graphics/material.h"



class EntityMesh : public Entity {

	public:
	EntityMesh() {};  //Create empty entityMesh
	EntityMesh(Mesh* mesh, Material* material, const std::string& name); //Create entityMesh
	~EntityMesh(); //desproy entityMesh

	// Attributes of the derived class  
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	bool isInstanced;

	bool IsInstanciated();

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void setMaterial(Material* material);
};


