#include "entityMesh.h"
#include "framework/camera.h"


void EntityMesh::render() {

	// Get the last camera that was activated 
	Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	material.shader->enable();
	//shader->enable(); TODO: Hace falta guardar un shader?
	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_viewproj", camera->projection_matrix);
	material.shader->setTexture("u_texture", texture, 1); //Third argument to revise
	if (material.diffuse) {
		material.shader->setUniform("u_texture", material.diffuse, 0);
	}
	material.shader->setUniform("u_model", getGlobalMatrix());


	// Render the mesh using the shader
	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	shader->disable();

	Entity::render(camera);
};

void EntityMesh::update(float elapsed_time) {
	Entity::update(elapsed_time);
}