
#include "entityPlayer.h"
#include "framework/input.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityMesh.h"
#include "framework/entities/entityCollider.h"
#include "framework/camera.h"
#include "game/game.h"
#include <string>

EntityPlayer::EntityPlayer(Mesh* mesh, Material material) : EntityMesh(mesh, material) {
	this->onFloor = TRUE;
	this->walkSpeed = 10.0f;
	this->velocity = Vector3(0,0,0);
	this->height = 3.0f;
	this->jumpSpeed = 25.0f;
	this->gravity = -9.81f * 2.0f;
	this->onFloor = true;
	this->is_jumping = false;
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

	Vector3 new_velocity = Vector3(0, 0, 0);
	Vector3 move_dir = Vector3(0, 0, 0);

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

	//Jump code
	if (Input::wasKeyPressed(SDL_SCANCODE_SPACE) && this->onFloor) {
		velocity.y += this->jumpSpeed;
		this->is_jumping = true;
		this->onFloor = false;
		this->gravity = -9.81f * 2.0f;
		this->time_for_groundpound = 0.3f;
	}
	//Ground pound
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && !this->onFloor && velocity.y < jumpSpeed - 5.0 && !this->ground_pound && this->time_for_groundpound < 0.0) {
		this->ground_pound = true;
		this->time_for_groundpound = 0.0;
		printf("groundpound\n");
	}
	//Dash
	if (Input::isKeyPressed(SDL_BUTTON_LEFT) && !this->is_dashing) {
		this->is_dashing = true;
		velocity += 5.0f * move_dir;
		//Tenemos que hacer un contador de 1 o 2 segundos que empiece al pulsar el dash y que cuando acabe le reste 5 a la velocidad y devuelva el bool a false
	}
	//printf("%f\n", gravity);
	if (move_dir.length() > 0) {
		move_dir.normalize();
	}
	move_dir *= speed_mult;
	new_velocity = velocity;
	new_velocity += move_dir;

	//We make sure that we don't pass a maximum speed
	if (abs(new_velocity.x) + abs(new_velocity.z) < 25)
		velocity = new_velocity;

	//If player is not colliding then we allow it to move
	Vector3 next_pos = position + velocity * elapsed_time;

	std::vector<sCollisionData> WallsCollisions;
	std::vector<sCollisionData> GroundCollisions;

	check_collision(next_pos, WallsCollisions, GroundCollisions);

	if (WallsCollisions.empty()) {
		position = next_pos;
	}
	else {
		this->onFloor = false;
		for (const sCollisionData& collision : WallsCollisions) {

			Vector3 newDir = velocity.dot(collision.colNormal) * collision.colNormal;
			// If normal is pointing upwards, it means it's a floor collision
			velocity.x -= newDir.x;
			velocity.y -= newDir.y;
			velocity.z -= newDir.z;
			//printf("%f %f %f \n", newDir.x, newDir.y, newDir.z);

		}
	}
	if (!GroundCollisions.empty()){
		for (const sCollisionData& collision : GroundCollisions) {
			float up_factor = collision.colNormal.dot(Vector3::UP);
			if (up_factor > 0.8f) {
				this->onFloor = true;
				if (collision.colPoint.y > (position.y + velocity.y * elapsed_time)) {
					position.y = collision.colPoint.y;
				}
			}
		}
	}

	// Apply gravity if the player is not on the floor
	if(this->onFloor) {
		velocity.y = 0.0f; // Reset Y velocity if on the floor
		this->is_jumping = false;
		this->ground_pound = false;
	}	
	else 
	{
		this->time_for_groundpound -= elapsed_time;
		if (this->ground_pound) {
			velocity.y = -100.0f;
		}
		else {
			velocity.y += gravity * elapsed_time;
		}
	}

	position += velocity * elapsed_time;

	//Reducimos velocity mientras no nos movemos (lentamente para que sea más smooth)
	if (move_dir.length() == 0) {
		velocity.x -= velocity.x * 2.5f * elapsed_time;
		velocity.z -= velocity.z * 2.5f * elapsed_time;
	}

	float offset = DEG2RAD * 90.0f;

	this->model.setTranslation(position);
	this->model.rotate(camera_yaw-offset, Vector3(0, 1, 0));

	Entity::update(elapsed_time);
}

void EntityPlayer::setMaterial(Material material) {
	EntityMesh::setMaterial(material);
}

void EntityPlayer::check_collision(Vector3 next_position, std::vector<sCollisionData>& WallsCollisions, std::vector<sCollisionData>& GroundCollisions) {
	for (Entity* ent : World::get_instance()->root->children) {
		EntityCollider* e = dynamic_cast<EntityCollider*>(ent);
		if (e != nullptr) {
			e->getCollisions(next_position, WallsCollisions, GroundCollisions);
		}
	}
}


