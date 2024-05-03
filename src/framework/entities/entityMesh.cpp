#include "entityMesh.h"
#include "framework/camera.h"



void EntityMesh::render(Matrix44 m) {

	// Get the last camera that was activated 
	Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	if (material) {
		material->shader->enable(); //TODO: Implementar el material en la funcion entityMesh, como el ejemplo no tiene lo he comentado
	}
	shader->enable();
	shader->setUniform("u_color", color);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_texture", texture, 1);
	shader->setUniform("u_model", m);
	shader->setUniform("u_time", time);
	if (material) {
		if (material->diffuse) {
			material->shader->setUniform("u_texture", material->diffuse, 0);
		}
		material->shader->setUniform("u_model", getGlobalMatrix());
	}

	// Render the mesh using the shader
	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	shader->disable();

	if (material) {
		material->shader->disable();
	}

	Entity::render(camera);
};

void EntityMesh::update(float elapsed_time) {
	Entity::update(elapsed_time);
}

void EntityMesh::setMaterial(Material* material) {
	this->material = material;
	this->shader = this->material->shader;
}