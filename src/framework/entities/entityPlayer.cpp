#include "entityPlayer.h"
#include "framework/camera.h"

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