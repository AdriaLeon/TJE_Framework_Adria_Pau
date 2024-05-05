#pragma once

#include "framework/includes.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"
#include "framework/camera.h"
#include "game.h"
#include <string>

class World {
public:
    static World* instance;
    std::vector<Entity*> entities;  //wa may create a superior class method or do a list for each type of entity I will leave like this at the moment
    Entity root;

    std::map<std::string, sRenderData> meshes_to_load;

    // Method to add or delete entities
    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);
    void removeAllEntities();

    // Method to redner all entities, this should be done calling just one function on entity since we will do the tree approach
    void renderAll(Camera* camera);

    // Method to update all entities
    void updateAll(float delta_time);

    bool parseScene(const char* filename, Entity* root);
    void renderEntities(Camera* camera);
};
