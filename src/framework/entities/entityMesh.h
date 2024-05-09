#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h"
#include "graphics/material.h"



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

	bool IsInstanciated();

	// Methods overwritten from base class
	void render(Camera* camera);
	void update(float elapsed_time);
	void setMaterial(Material material);
};
/*
class EntityPlayer : public EntityMesh {
	public:
		Vector3 position;

		void update(float elapsed_time);
};
*/