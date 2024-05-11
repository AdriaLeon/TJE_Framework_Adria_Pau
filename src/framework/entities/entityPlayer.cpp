
#include "entityPlayer.h"
#include "framework/camera.h"
#include "game/world.h"

EntityPlayer::EntityPlayer(Mesh* mesh, Material material){
	this->mesh = mesh;
	this->onFloor = TRUE;
	this->material = material;
}

void EntityPlayer::render(Camera* camera) {

	if (!mesh)		return;

	if (!material.shader) {
		material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	}

	// Enable shader and pass uniforms 
	material.shader->enable();
	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_camera_position", camera->eye);
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material.shader->setUniform("u_time", time);


	if (material.diffuse) {
		material.shader->setUniform("u_texture", material.diffuse, 0);
	}
	else {
		material.shader->setUniform("u_texture", material.diffuse->getWhiteTexture(), 0);
	}

	mesh->render(GL_TRIANGLES);
	// Disable shader after finishing rendering
	material.shader->disable();
};

void EntityPlayer::update(float elapsed_time) {

	float camera_yaw = World::get_instance()->camera_yaw;

	Entity::update(elapsed_time);
}

void EntityPlayer::setMaterial(Material material) {
	this->material = material;
}