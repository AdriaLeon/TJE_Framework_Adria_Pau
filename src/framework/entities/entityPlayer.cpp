
#include "entityPlayer.h"
#include "framework/input.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityMesh.h"
#include "framework/entities/entityCollider.h"
#include "framework/camera.h"
#include "game/game.h"
#include "framework/audio.h"
#include <string>

EntityPlayer::EntityPlayer(Mesh* mesh, Material material) : EntityMesh(mesh, material, "Idle") {
	this->onFloor = TRUE;
	this->walkSpeed = 8.0f;
	this->velocity = Vector3(0,0,0);
	this->height = 3.5f;
	this->jumpSpeed = 20.0f;
	this->gravity = -20.0f;
	this->onFloor = true;
	this->is_jumping = false;
	this->ground_pound = false;
	this->move_xz = true;
	this->dash_cooldown = 0.0;
	this->coyoteTime = 0.2f;
	this->timeSinceGrounded = 0.0f;
	this->groundPoundChannel = 0;
	this->hasLanded = false;
	entityType = eEntityType::PLAYER;
}

void EntityPlayer::render(Camera* camera) {
	EntityMesh::render(camera);
};





void EntityPlayer::update(float elapsed_time) {

	float camera_yaw = World::get_instance()->camera_yaw;
	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));

	bool in_the_air = !this->onFloor;

	//Guardamos speed_mult a parte por si queremos hacer un botón de sprint
	float speed_mult = this->walkSpeed;

	Vector3 position = model.getTranslation();

	Vector3 new_velocity = Vector3(0, 0, 0);
	Vector3 move_dir = Vector3(0, 0, 0);

	handle_inputs(move_dir, mYaw, position);

	//printf("%f\n", gravity);
	if (move_dir.length() > 0) {
		move_dir.normalize();
		if (!this->onFloor) //When in the air its harder to turn around
			move_dir = move_dir / 2.0f;
	}

	move_dir *= speed_mult;
	new_velocity = velocity + move_dir;

	if (this->onFloor) {
		FreezeAnimation(false);
		if (new_velocity.length() <= 5.0f) {
			if (animator->getCurrentAnimation()->name != GetAnimationPath("Idle"))
				PlayAnimation("Idle");
		}
		else {
			if (animator->getCurrentAnimation()->name != GetAnimationPath("Running"))
				PlayAnimation("Running");
		}
	}
	else {
		if (this->wait_to_finish_animation < 0.0f) {
			if (animator->getCurrentAnimation()->name != GetAnimationPath("After_Jump"))
				PlayAnimation("After_Jump", true, 1.0f);
		}
		else {
			this->wait_to_finish_animation -= elapsed_time;
			this->wait_to_freeze -= elapsed_time;
		}
	}

	// Calculate the horizontal speed (consider only x and z components)
	float horizontal_speed = sqrt(new_velocity.x * new_velocity.x + new_velocity.z * new_velocity.z);

	if (horizontal_speed < this->max_speed) {
		velocity = new_velocity;
	} 
	else {
		float vertical_component = new_velocity.y;
		Vector3 horizontal_velocity = Vector3(new_velocity.x, 0.0f, new_velocity.z).normalize() * max_speed;
		velocity = Vector3(horizontal_velocity.x, vertical_component, horizontal_velocity.z);
	}

	if (this->is_dashing) {
		if (this->dash_timer > 0.0){
			velocity = move_dir * 30.0f;
			this->dash_timer -= elapsed_time;
		}
		else {
			this->is_dashing = false;
			this->dash_timer = 0.0;
			velocity = velocity / 5.0f;
		}
	}

	if (this->onFloor) {
		this->timeSinceGrounded = 0.0f;
	}
	else {
		this->timeSinceGrounded += elapsed_time;
	}

	// Apply gravity if the player is not on the floor
	if (!this->onFloor) {
		this->time_for_groundpound -= elapsed_time;
		if (this->ground_pound) {
			velocity.y = -100.0f;
		}
		else {
			velocity.y += gravity * elapsed_time;
		}
	}
	else {
		velocity.y = -2.0f;
	}

	Vector3 final_vel = velocity;
	if (this->sprinting) //When sprinting x and z velocity double  (Do it so that you maintain sprinting when you started sprinting in the ground but 
		//you cannot start sprinting in the air)
		final_vel.x *= 2.0f;
		final_vel.z *= 2.0f;

	//If player is not colliding then we allow it to move
	Vector3 next_pos = position + final_vel * elapsed_time;

	std::vector<sCollisionData> WallsCollisions;
	std::vector<sCollisionData> GroundCollisions;
	std::vector<sCollisionData> FastCollisions;

	check_collision(next_pos, WallsCollisions, GroundCollisions);
	if (velocity.length() > 25) {
		check_collisionHightVelocity(position, next_pos, FastCollisions);
	}
	handle_collisions(FastCollisions, WallsCollisions, GroundCollisions, position, elapsed_time, final_vel);

	position += final_vel * elapsed_time;

	if (in_the_air == this->onFloor) {
		this->dash_cooldown = 0.0f;
		this->wait_to_finish_animation = 0.15f;
	}
	else if (this->onFloor) {
		this->dash_cooldown -= elapsed_time;
	}

	//Reducimos velocity mientras no nos movemos (lentamente para que sea más smooth)
	if (move_dir.length() == 0 && this->onFloor) {
		velocity.x -= velocity.x * 15.0f * elapsed_time;
		velocity.z -= velocity.z * 15.0f * elapsed_time;
	}

	float offset = DEG2RAD * 90.0f;

	this->model.setTranslation(position);
	this->model.rotate(camera_yaw-offset, Vector3(0, 1, 0));

	//std::cout << "onFloor: " << this->onFloor << std::endl;

	EntityMesh::update(elapsed_time);
}

void EntityPlayer::check_collision(Vector3 next_position, std::vector<sCollisionData>& WallsCollisions, std::vector<sCollisionData>& GroundCollisions) {
	for (Entity* ent : World::get_instance()->root->children) {
		EntityCollider* e = dynamic_cast<EntityCollider*>(ent);
		if (e != nullptr) {
			e->getCollisions(next_position, WallsCollisions, GroundCollisions);
		}
	}
}

void EntityPlayer::check_collisionHightVelocity(Vector3 position, Vector3 next_position, std::vector<sCollisionData>& Collisions) {
	for (Entity* ent : World::get_instance()->root->children) {
		EntityCollider* e = dynamic_cast<EntityCollider*>(ent);
		if (e != nullptr) {
			e->getCollisionsHightVelocity(position, next_position, Collisions);
		}
	}
}

void EntityPlayer::handle_collisions(std::vector<sCollisionData> FastCollisions, std::vector<sCollisionData> WallsCollisions, std::vector<sCollisionData> GroundCollisions, Vector3 &position, float elapsed_time, Vector3& final_vel) {
	
	if (!FastCollisions.empty()) {
		for (const sCollisionData& collision : FastCollisions) {

			if(!this->ground_pound)
				this->is_dashing = false;

			Vector3 newDir = final_vel.dot(collision.colNormal) * collision.colNormal;
			// If normal is pointing upwards, it means it's a floor collision
			final_vel.x -= newDir.x;
			final_vel.z -= newDir.z;

			this->is_dashing = false;

			float up_factor = collision.colNormal.dot(Vector3::UP);
			if (up_factor < -0.7f && newDir.y > 0.0)
				final_vel.y -= newDir.y;
		}
	}

	if (!WallsCollisions.empty()) {
		for (const sCollisionData& collision : WallsCollisions) {

			Vector3 newDir = final_vel.dot(collision.colNormal) * collision.colNormal;
			// If normal is pointing upwards, it means it's a floor collision
			final_vel.x -= newDir.x;
			final_vel.z -= newDir.z;

			// Project the velocity onto the plane formed by the collision normal and the ground plane
			Vector3 planeNormal = collision.colNormal.cross(Vector3::UP).normalize();
			Vector3 projectedVel = final_vel - planeNormal * final_vel.dot(planeNormal);

			// Apply the projected velocity to the position
			Vector3 next_pos = position + projectedVel * elapsed_time;
			Vector3 toCollisionPointXZ = Vector3(collision.colPoint.x - next_pos.x, 0.0f, collision.colPoint.z - next_pos.z);
			float distanceXZ = toCollisionPointXZ.dot(Vector3(collision.colNormal.x, 0.0f, collision.colNormal.z));
			if (distanceXZ < 0) { // Character is penetrating the collision surface along x and z
				Vector3 adjustment = collision.colNormal * distanceXZ * 0.1;
				position.x -= adjustment.x;
				position.z -= adjustment.z;
			}

			float up_factor = collision.colNormal.dot(Vector3::UP);
			//Check collision with ceiling
			if (up_factor < -0.7f && newDir.y > 0.0)
				final_vel.y -= newDir.y;
		}
	}

	if (!GroundCollisions.empty()) {
		for (const sCollisionData& collision : GroundCollisions) {
			float up_factor = collision.colNormal.dot(Vector3::UP);
			this->onFloor = true;
			this->ground_pound = false;
			this->is_jumping = false;
			final_vel.y = 0.0f;
			if (up_factor > 0.7f) {
				if (collision.colPoint.y > (position.y + final_vel.y * elapsed_time)) {
					position.y = collision.colPoint.y;
				}
			}
			if (!hasLanded) {
				Audio::Play("data/sounds/OnGround.wav", 0.1);
				this->hasLanded = true;
			}
		}
	}
	else {
		this->onFloor = false;
		this->hasLanded = false;
	}

	velocity = final_vel;
	if (this->sprinting)
		velocity.x = final_vel.x / 2.0f;
		velocity.z = final_vel.z / 2.0f;
}

void EntityPlayer::setMaterial(Material material) {
	EntityMesh::setMaterial(material);
}

void EntityPlayer::handle_inputs(Vector3& move_dir, Matrix44 mYaw, Vector3&position) {

	Vector3 front = Vector3(1, 0, 0);

	// Rotate the front vector according to the camera yaw
	front = mYaw * front;

	// Calculate the right vector based on the rotated front vector
	Vector3 right = front.cross(Vector3(0, 1, 0));

	if (!this->ground_pound) {
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
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_O)) { //Debugging tool to know the players position
		printf("%f %f %f\n", this->model.getTranslation().x, this->model.getTranslation().y, this->model.getTranslation().z);
	}

	//Añado un boton de correr por si hay que probar cosas, en teoria la version final no tendra
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) {
		this->sprinting = true;
	}
	else {
		this->sprinting = false;
	}

	//Jump code
	if (Input::wasKeyPressed(SDL_SCANCODE_SPACE) && (this->onFloor || this->timeSinceGrounded <= this->coyoteTime) && !this->is_jumping) {
		velocity.y += this->jumpSpeed;
		this->is_jumping = true;
		this->onFloor = false;
		this->gravity = -9.81f * 2.0f;
		this->time_for_groundpound = 0.6f;
		position.y += 0.4f;
		Audio::Play("data/sounds/Jump.wav", 0.2);
		if (animator->getCurrentAnimation()->name != GetAnimationPath("Jump")) {
			PlayAnimation("Jump", false);
		}
	}
	//Ground pound
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && !this->onFloor && !this->ground_pound && this->time_for_groundpound < 0.0) {
		this->ground_pound = true;
		this->time_for_groundpound = 0.0;
		velocity.x = 0.0f;
		velocity.z = 0.0f;
		if (groundPoundChannel != 0) {
			Audio::Stop(groundPoundChannel);
			groundPoundChannel = 0;
		}
		groundPoundChannel = Audio::Play("data/sounds/GroundPound.wav", 0.3);
	}
	//Dash
	if (!this->is_dashing && Input::isMousePressed(SDL_BUTTON_LEFT) && this->dash_cooldown <= 0.0f && !this->ground_pound) { //Input::isKeyPressed(SDL_BUTTON_LEFT)) {
		this->is_dashing = true;
		this->dash_timer = 0.2;
		this->dash_cooldown = 0.2;
		Audio::Play("data/sounds/Dash.wav", 0.3);
		//Tenemos que hacer un contador de 1 o 2 segundos que empiece al pulsar el dash y que cuando acabe le reste 5 a la velocidad y devuelva el bool a false
	}
}