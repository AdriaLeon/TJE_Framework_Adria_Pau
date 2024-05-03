#include "entityMesh.h"
#include "framework/camera.h"

EntityMesh::EntityMesh(Mesh* mesh, Material* material, const std::string& name)
{
	this->mesh = mesh;
	this->material = material;
	this->name = name;
	this->isInstanced = true;
}


EntityMesh::~EntityMesh() { };

//Checks if we are using the entity 
bool EntityMesh::IsInstanciated() {
	return isInstanced;
}

void EntityMesh::render(Camera* camera) {

	if (!mesh || !material)		return;

	if (!material->shader) {
		material->shader = Shader::Get(isInstanced ? ("data/shaders/basic.vs", "data/shaders/texture.fs") : ("data/shaders/basic.vs", "data/shaders/flat.fs"));
	}

	// Enable shader and pass uniforms 
	material->shader->enable();
	material->shader->setUniform("u_color", material->color);
	material->shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material->shader->setUniform("u_model", getGlobalMatrix());
	material->shader->setUniform("u_time", time);
	if (material) {
		if (material->diffuse) {
			material->shader->setUniform("u_texture", material->diffuse, 0);
		}
	}

	// Render the mesh using the shader
	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	material->shader->disable();

	Entity::render(camera);
};

void EntityMesh::update(float elapsed_time) {
	Entity::update(elapsed_time);
}

void EntityMesh::setMaterial(Material* material) {
	this->material = material;
}





//In case we want to add level of detail depending on the distance

/*
std::vector<Matrix44> models;

std::vector<sMeshLOD> meshLods;

void EntityMesh::addLOD(Mesh* mesh, float distance)
{
	meshLods.push_back({ mesh, distance });

	std::sort(meshLods.begin(), meshLods.end(), [](const sMeshLOD& lod1, const sMeshLOD& lod2) {
		return lod1.distance > lod2.distance;
		});
}

void EntityMesh::addInstance(const Matrix44& model)
{
	models.push_back(model);
}
*/