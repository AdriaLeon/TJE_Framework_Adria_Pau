#include "entityMesh.h"
#include "framework/camera.h"

EntityMesh::EntityMesh(Mesh* mesh, Material material) 
{
	this->mesh = mesh;
	entityType = eEntityType::MESH;

	this->material = material;
	this->isInstanced = false;
}
EntityMesh::EntityMesh(Mesh* mesh, Shader* shader, Texture* texture, const std::string& name)
{
	this->mesh = mesh;
	entityType = eEntityType::MESH;
	this->material.diffuse = texture;
	this->material.shader = shader;
	this->name = name;
	this->isInstanced = false;
}

EntityMesh::EntityMesh(char* Smesh, char* shaderVs, char* shaderFs, char* Stexture, const std::string& name)
{


	// Load one texture using the Texture Manager
	Texture* texture = Texture::Get(Stexture);

	// Example of shader loading using the shaders manager
	Shader* shader = Shader::Get(shaderVs, shaderFs);

	// Example of loading Mesh from Mesh Manager
	Mesh* mesh = Mesh::Get(Smesh);

	this->mesh = mesh;
	this->material.diffuse = texture;
	this->material.shader = shader;
	this->name = name;
	this->isInstanced = false;
}


EntityMesh::~EntityMesh() { };

//Checks if we are using the entity 
bool EntityMesh::IsInstanciated() {
	return isInstanced;
}

void EntityMesh::render(Camera* camera) {

	if (!mesh)		return;

	if (!material.shader) {
		material.shader = Shader::Get(isInstanced ? ("data/shaders/basic.vs", "data/shaders/texture.fs") : ("data/shaders/basic.vs", "data/shaders/flat.fs"));
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


	if (!isInstanced){

		const Matrix44& globalMatrix = getGlobalMatrix();

		// Compute bounding sphere center in world coords
		Vector3 sphere_center = globalMatrix * mesh->box.center;
		Vector3 halfsize = globalMatrix * mesh->box.halfsize;

		// Discard objects whose bounding sphere is not inside the camera frustum
		if ((!camera->testBoxInFrustum(sphere_center, halfsize) ||
			camera->eye.distance(sphere_center) > 5000.0f))
			return;

		material.shader->setUniform("u_model", globalMatrix);
		// Render the mesh using the shader
		mesh->render(GL_TRIANGLES);
	} 
	else {

		// Iterate by instance and only upload the data that is different, and do the drawcall
		for (int i = 0; i < models.size(); ++i)
		{
			const Matrix44& instanceGlobalMatrix = models[i];

			// Compute bounding sphere center for this instance
			Vector3 instance_sphere_center = instanceGlobalMatrix * mesh->box.center;
			Vector3 halfsize = instanceGlobalMatrix * mesh->box.halfsize;

			// Discard instance if its bounding sphere is not inside the camera frustum
			if (!camera->testBoxInFrustum(instance_sphere_center, halfsize) ||
				camera->eye.distance(instance_sphere_center) > 5000.0f)
				continue;

			models_to_render.push_back(models[i]);
		}
		mesh->renderInstanced(GL_TRIANGLES, models_to_render.data(), models_to_render.size());

		models_to_render.clear();
	}

	// Disable shader after finishing rendering
	material.shader->disable();

	Entity::render(camera);
};

void EntityMesh::update(float elapsed_time) {
	Entity::update(elapsed_time);
}

void EntityMesh::setMaterial(Material material) {
	this->material = material;
}


///Powerpoint 9 implementar si se necessita
//TO DO


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

/*
void EntityPlayer::update(float elapsed_time) {
	/*float camera_yaw = World::get_instance()->camera_yaw;

	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0,1,0));


	float camera_yaw = World::get_instance()->camera_yaw;

	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0,1,0));


	Vector3 front = Vector3(0, 0, -1);
	Vector3 right = Vector3(1, 0, 0);

	Vector3 position = model.getTranslation();
	Vector3 move_dir;
	//TODO: Normalizar move_dir para los movimientos diagonales
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) {
		move_dir += front;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) {
		move_dir -= front;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
		move_dir += right;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
		move_dir -= right;
	}
	float speed_mult = 10; //TODO: Definir velocidad del jugador

}

*/