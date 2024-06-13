#pragma once

#include "framework/includes.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityMesh.h"
#include "framework/entities/entityPlayer.h"
#include "framework/entities/entityCollider.h"
#include "framework/entities/entityui.h"
#include "framework/camera.h"
#include "framework/audio.h"
#include "game.h"
#include <string>



class World {
public:
    static World* instance;
    std::vector<Entity*> entities;  //we may create a superior class method or do a list for each type of entity I will leave like this at the moment
    Entity* root;
    float camera_yaw;
    float camera_pitch;
    Camera* camera2D;
    int current_check_point = 0;
    std::vector<Vector3> checkpoints = { Vector3(0.0f, -3.4f, 0.0f), Vector3(242.0f, -4.0f, 3.5f), Vector3(522.0f, 28.0f, 8.0f), Vector3(870.0f, -109.7f, 7.0f), Vector3(1411.67f, - 71.9f, 32.1f), Vector3(1895.71f, - 76.92f, - 25.55f)};
    std::vector<EntityUI*> ui_elements;
    EntityMesh* landscape;
    EntityPlayer* player;
    HCHANNEL channelBG;

    //Constructor
    World();

    // Method to add or delete entities
    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);
    void removeAllEntities();
    void setCamerayaw(Camera* camera);

    // Method to redner all entities, this should be done calling just one function on entity since we will do the tree approach
    void renderAll(Camera* camera);

    // Method to update all entities
    void updateAll(float delta_time);

    bool parseScene(const char* filename);
    void loadCubeMap();
    void loadPlayer();
    void renderEntities(Camera* camera);
    void updateCubemap(Camera* camera);
    static World* get_instance();
    void check_chekpoints();
    void loadAudios();

    //bool check_player_collisions(Vector3& target_pos);
};
