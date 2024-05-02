#include "stage.h"

void Stage::addEntity(Entity* entity) {
    entities.push_back(entity);
}

void Stage::removeEntity(Entity* entity) {
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end()) {
        entities.erase(it);
    }
}