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

	if (material.diffuse) {
		material.shader = Shader::Get(("data/shaders/basic.vs", "data/shaders/texture.fs"));
	}
	else {
		material.shader = Shader::Get(("data/shaders/basic.vs", "data/shaders/color.fs"));
	}
	// Enable if we want to do transparency
	// glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

	World* world = World::get_instance();
	Matrix44 viewProj = world->camera2D->viewprojection_matrix;

	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_viewprojection", viewProj);
	material.shader->setUniform("u_color", material.color);

	if (material.diffuse) {
		material.shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		material.shader->setUniform("u_texture", material.diffuse, 0);
	}
	else {
		material.shader->setUniform("u_color", material.color);
		material.shader->setUniform("u_texture", Texture::getWhiteTexture(), 0);
	}

	const Matrix44& globalMatrix = getGlobalMatrix();

	// Compute bounding sphere center in world coords
	Vector3 sphere_center = globalMatrix * mesh->box.center;
	Vector3 halfsize = globalMatrix * mesh->box.halfsize;

	// Discard objects whose bounding sphere is not inside the camera frustum
	if ((!camera->testBoxInFrustum(sphere_center, halfsize) ||
		camera->eye.distance(sphere_center) > 2000.0f))
		return;

	material.shader->setUniform("u_model", globalMatrix);

	mesh->render(GL_TRIANGLES);

	material.shader->disable();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}