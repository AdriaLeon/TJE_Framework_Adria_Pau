
#include "entityPlayer.h"
#include "framework/input.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityMesh.h"
#include "framework/camera.h"
#include "game/game.h"
#include "game/world.h"
#include <string>

EntityPlayer::EntityPlayer(Mesh* mesh, Material* material){
	this->mesh = mesh;
	this->onFloor = TRUE;
	this->material = material;
	this->walkSpeed = 10.0f;
	this->velocity = Vector3(0,0,0);
	entityType = eEntityType::PLAYER;
}

void EntityPlayer::render(Camera* camera) {

	if (!mesh)		return;

	if (!material->shader) {
		material->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	}

	// Enable shader and pass uniforms 
	material->shader->enable();
	material->shader->setUniform("u_color", material->color);
	material->shader->setUniform("u_camera_position", camera->eye);
	material->shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material->shader->setUniform("u_time", time);


	if (material->diffuse) {
		material->shader->setUniform("u_texture", material->diffuse, 0);
	}
	else {
		material->shader->setUniform("u_texture", material->diffuse->getWhiteTexture(), 0);
	}
	const Matrix44& globalMatrix = getGlobalMatrix();

	// Compute bounding sphere center in world coords
	Vector3 sphere_center = globalMatrix * mesh->box.center;
	Vector3 halfsize = globalMatrix * mesh->box.halfsize;

	// Discard objects whose bounding sphere is not inside the camera frustum
	if ((!camera->testBoxInFrustum(sphere_center, halfsize) ||
		camera->eye.distance(sphere_center) > 5000.0f))
		return;

	material->shader->setUniform("u_model", globalMatrix);
	// Render the mesh using the shader
	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	material->shader->disable();
};

void EntityPlayer::update(float elapsed_time) {

	float camera_yaw = World::get_instance()->camera_yaw;

	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));
	Vector3 front = Vector3(0, 0, -1);
	Vector3 right = Vector3(1, 0, 0);

	//rotacion inicial para eliminar el desfase inicial
	Matrix44 initial_rotation;
	initial_rotation.rotate(DEG2RAD * 90.0f, Vector3(0, 1, 0));
	model = initial_rotation * model;

	//Guardamos speed_mult a parte por si queremos hacer un botón de sprint
	float speed_mult = this->walkSpeed; 

	Vector3 position = model.getTranslation();

	Vector3 new_velocity;
	Vector3 move_dir;

	if (Input::isKeyPressed(SDL_SCANCODE_W)) {// || Input::isKeyPressed(SDL_SCANCODE_UP)) {
		move_dir += front;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_S)) {// || Input::isKeyPressed(SDL_SCANCODE_DOWN)) {
		move_dir -= front;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_A)) {// || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
		move_dir -= right;
	}
	if (Input::isKeyPressed(SDL_SCANCODE_D)) {// || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
		move_dir += right;
	}
	//Añado un boton de correr por si hay que probar cosas, en teoria la version final no tendra
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) {
		speed_mult *= 3.0f;
	}
	move_dir.normalize();
	move_dir *= speed_mult;
	new_velocity = velocity;
	new_velocity += move_dir;
	if (abs(new_velocity.x) + abs(new_velocity.z) < 25)
		velocity = new_velocity;
	position += velocity * elapsed_time;

	//Reducimos velocity mientras no nos movemos (lentamente para que sea más smooth)
	float velocity_x_reduction = velocity.x * 2.5f * elapsed_time;
	float velocity_z_reduction = velocity.z * 2.5f * elapsed_time;
	velocity.x -= velocity_x_reduction;
	velocity.z -= velocity_z_reduction;

	float offset = DEG2RAD * 90.0f;

	this->model.setTranslation(position);
	this->model.rotate(camera_yaw-offset, Vector3(0, 1, 0));

	Entity::update(elapsed_time);
}

void EntityPlayer::setMaterial(Material* material) {
	this->material = material;
}