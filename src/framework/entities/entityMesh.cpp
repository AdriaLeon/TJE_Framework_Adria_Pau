#include "entityMesh.h"
#include "framework/camera.h"

void EntityMesh::render() {

	// Get the last camera that was activated 
	Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewproj", camera->vp);
	shader->setTexture("u_texture", texture);


	// Render the mesh using the shader
	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	shader->disable();
};

void EntityMesh::update(float elapsed_time) {
	Entity::update(elapsed_time);
}