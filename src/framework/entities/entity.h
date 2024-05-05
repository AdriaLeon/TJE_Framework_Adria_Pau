#pragma once

#include "framework/includes.h"
#include "framework/framework.h"

class Camera;

enum class eEntityType {
	MESH,
	PLAYER
};

class Entity {

public:

	Entity() {}; 			// Constructor
	virtual ~Entity() {}; 	// Destructor

	std::string name;

	Matrix44 model;
	std::vector<Matrix44> models;

	Entity* parent = nullptr;
	std::vector<Entity*> children;
	std::vector<Entity*> s_to_destroy;

	void addChild(Entity* child);
	void removeChild(Entity* child);

	// Methods that should be overwritten
	// by derived classes 
	virtual void render(Camera* camera);
	virtual void update(float delta_time);

	// Some useful methods
	Matrix44 getGlobalMatrix();
	float distance(Entity* e);

	void destroy(); //cleans all s_to_destroy vector
	void cleanupEntities();

	// Getter and setter for entityType
	eEntityType getEntityType() const {
		return entityType;
	}

	eEntityType entityType;
};
