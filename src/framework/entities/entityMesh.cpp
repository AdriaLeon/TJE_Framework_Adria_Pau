#include "entityMesh.h"
#include "framework/camera.h"

EntityMesh::EntityMesh(Mesh* mesh, Material material) 
{
	this->mesh = mesh;
	entityType = eEntityType::MESH;

	this->material = material;
	this->isInstanced = false;
	this->animator = nullptr;
}

EntityMesh::EntityMesh(Mesh* mesh, Material material, const std::string& initial_animation)
{
	this->mesh = mesh;
	entityType = eEntityType::MESH;

	this->material = material;
	this->isInstanced = false;

	ItsAnimated();
	PlayAnimation(initial_animation);
}

EntityMesh::EntityMesh(Mesh* mesh, Shader* shader, Texture* texture, const std::string& name)
{
	this->mesh = mesh;
	entityType = eEntityType::MESH;
	this->material.diffuse = texture;
	this->material.shader = shader;
	this->name = name;
	this->isInstanced = false;
	this->animator = nullptr;
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
	this->animator = nullptr;
}


EntityMesh::~EntityMesh() { };

//Checks if we are using the entity 
bool EntityMesh::IsInstanciated() {
	return isInstanced;
}

void EntityMesh::render(Camera* camera) {

	Animation* anim = nullptr;
	Mesh* mesh_anim = nullptr;

	if (!mesh)		return;

	if (!material.shader) {
		if (animated) {
			material.shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/color.fs");
		}
		else {
			if (material.diffuse) {
				material.shader = Shader::Get(("data/shaders/basic.vs", "data/shaders/texture.fs"));
			}
			else {
				material.shader = Shader::Get(("data/shaders/basic.vs", "data/shaders/color.fs"));
			}
		}
	}


	// Enable shader and pass uniforms 
	material.shader->enable();
	material.shader->setUniform("u_camera_position", camera->eye);
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material.shader->setUniform("u_time", time);

	if (use_pong) {
		material.shader->setUniform("ambient_light", vec3(0.4f, 0.5f, 0.6f));
		material.shader->setUniform("u_light_color", vec3(0.9f, 0.9f, 1.0f) * 2.0f);
		material.shader->setUniform("u_light_direction", vec3(-1.0f, 1.0f, 0.0f));
		material.shader->setUniform("shininess", 32.0f);
	}

	if (material.diffuse) {
		material.shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		material.shader->setUniform("u_texture", material.diffuse, 0);

		if (use_pong) {
			material.shader->setUniform("u_Ka", material.ka);
			material.shader->setUniform("u_Kd", material.kd);
			material.shader->setUniform("u_Ks", material.ks);
		}
	}
	else {
		material.shader->setUniform("u_color", material.color);
		material.shader->setUniform("u_texture", Texture::getWhiteTexture(), 0);
	}

	if (fog) {
		material.shader->setUniform("fogMaxDist", 1000.0f);
		material.shader->setUniform("fogMinDis", 100.0f);
		material.shader->setUniform("fog_color", vec3(0.7f, 0.7f, 0.7f));
	}


	if (!isInstanced){

		const Matrix44& globalMatrix = getGlobalMatrix();

		// Compute bounding sphere center in world coords
		Vector3 sphere_center = globalMatrix * mesh->box.center;
		Vector3 halfsize = globalMatrix * mesh->box.halfsize;

		// Discard objects whose bounding sphere is not inside the camera frustum
		if ((!camera->testBoxInFrustum(sphere_center, halfsize) ||
			camera->eye.distance(sphere_center) > 2000.0f))
			return;

		material.shader->setUniform("u_model", globalMatrix);

		if (animated) {
			mesh->renderAnimated(GL_TRIANGLES, &animator->getCurrentSkeleton());
		}
		else {
			// Render the mesh using the shader
			mesh->render(GL_TRIANGLES);
		}
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
				camera->eye.distance(instance_sphere_center) > 2000.0f)
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
	if (animated) {
		animator->update(elapsed_time);
	}
	Entity::update(elapsed_time);
}

void EntityMesh::setMaterial(Material material) {
	this->material = material;
}

void EntityMesh::PlayAnimation(const std::string& name, bool loop, float transition_time) {
	std::string filename_animation_str = "data/Animations/" + name + ".skanim";
	const char* filename_animation = filename_animation_str.c_str();
	animator->freezeAnimation(false);

	animator->playAnimation(filename_animation, loop, transition_time);
}

std::string EntityMesh::GetAnimationPath(const std::string& name) {
	return "data/Animations/" + name + ".skanim";
}

void EntityMesh::FreezeAnimation(bool freeze) {
	animator->freezeAnimation(freeze);
}

void EntityMesh::ItsAnimated() {
	animated = true;
	animator = new Animator();
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
Clase de shaders:

material.shader->setUniform(u_time/u_camera_position_u_light_position/u_fog_factor, parámetro): Rellenamos el shader con valores arbitrarios de los parametros a utilizar.
modificación a material.h si hacer falta.

Vector2 maps = {0.0f, 0.0f}: Vector usado como bools para indicar cuales usamos y cuales no
if (material.normal)
	maps.x = 1.0
	material.shader->setUniform(u_normal_texture, material.normal, 1)


En el shader recibimos estos parámetros, está explicando el de phong, se supone que el código está en el aula global.
*/
