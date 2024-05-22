
#include "entityPlayer.h"
#include "framework/input.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityMesh.h"
#include "framework/camera.h"
#include "game/game.h"
#include "game/world.h"
#include <string>

EntityPlayer::EntityPlayer(Mesh* mesh, Material material) : EntityMesh(mesh, material) {
	this->onFloor = TRUE;
	this->walkSpeed = 10.0f;
	this->velocity = Vector3(0,0,0);
	this->height = 3.0f;
	entityType = eEntityType::PLAYER;
}

void EntityPlayer::render(Camera* camera) {

	EntityMesh::render(camera);
	Mesh* mesh = Mesh::Get("data/meshes/sphere.obj");
	Matrix44 m = model;
	float sphere_radius = 1.0f;
	material.shader->enable();

	m.translate(0.0f, 1.5f, 0.0f);
	m.scale(sphere_radius, sphere_radius, sphere_radius);

	material.shader->setUniform("u_color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material.shader->setUniform("u_model", m);

	mesh->render(GL_LINES);

	material.shader->disable();
};





void EntityPlayer::update(float elapsed_time) {

	float camera_yaw = World::get_instance()->camera_yaw;

	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));
	Vector3 front = Vector3(1, 0, 0);

	// Rotate the front vector according to the camera yaw
	front = mYaw * front;

	// Calculate the right vector based on the rotated front vector
	Vector3 right = front.cross(Vector3(0, 1, 0));
	//Vector3 right = Vector3(1, 0, 0);


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
	if (Input::wasKeyPressed(SDL_SCANCODE_O)) { //Debugging tool to know the players position
		printf("%f %f %f", this->model.getTranslation().x, this->model.getTranslation().y, this->model.getTranslation().z);
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

	// Apply gravity if the player is not on the floor
	//float gravity = -9.8f;
	//velocity.y += gravity * elapsed_time; // Update velocity with gravity
	if (!check_collision(Vector3(position.x, position.y - 9.8 * elapsed_time, position.z))) {//this->onFloor) {
		float gravity = -9.8f;
		velocity.y += gravity * elapsed_time; // Update velocity with gravity
	}
	else {
		velocity.y = 0.0f; // Reset Y velocity if on the floor
	}
	
	Vector3 next_pos = position + velocity * elapsed_time;
	if (!check_collision(next_pos)) {
		position = next_pos;
	}
	else {

		for (const sCollisionData& collision : World::get_instance()->collisions) {

			// If normal is pointing upwards, it means it's a floor collision
			float up_factor = collision.colNormal.dot(Vector3::UP);
			if (up_factor > 0.8) {
				continue;
			}

			// Move along wall when colliding
			Vector3 newDir = velocity.dot(collision.colNormal) * collision.colNormal;
			velocity.x -= newDir.x;
			velocity.y -= newDir.y;
			velocity.z -= newDir.z;
		}

		position += velocity * elapsed_time;
	}
	

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

void EntityPlayer::setMaterial(Material material) {
	EntityMesh::setMaterial(material);
}

bool EntityPlayer::check_collision(Vector3 next_position) {
	World* world = World::get_instance();
	world->collisions.clear();
	if (world->check_player_collisions(next_position, world->collisions)) {
		return true;
	}
	return false;
}


