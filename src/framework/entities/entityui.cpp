#include "entityui.h"
#include "framework/entities/entity.h"
#include "game/game.h"
#include "framework/camera.h"
#include <string>

EntityUI::EntityUI(Vector2 s, const Material& mat) {
	this->mesh = new Mesh();
	mesh->createQuad(0.0, 0.0, s.x, s.y, false);
	this->material = mat;
}

void EntityUI::Render(Camera* camera) {
	if (!visible) return;
	EntityMesh::render(camera);
}