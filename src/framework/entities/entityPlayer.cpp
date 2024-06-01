
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
	this->walkSpeed = 8.0f;
	this->velocity = Vector3(0,0,0);
	this->height = 3.0f;
	this->jumpSpeed = 20.0f;
	this->gravity = -20.0f;
	this->onFloor = true;
	this->is_jumping = false;
	this->ground_pound = false;
	this->move_xz = true;
	this->dash_cooldown = 0.0;
	this->coyoteTime = 0.3f;
	this->timeSinceGrounded = 0.0f;
	entityType = eEntityType::PLAYER;
}

void EntityPlayer::render(Camera* camera) {

	EntityMesh::render(camera);
	/*
	Mesh* mesh = Mesh::Get("data/meshes/sphere.obj");

	float sphere_radius = 0.2f;
	float distance = 2.0f * sphere_radius; // Distance between the centers of touching spheres
	Vector3 center = Vector3(0.0f, height - 0.2f, 0.0f);

	// Generate directions using sin and cos
	std::vector<Vector3> directions;
	for (int i = 0; i < 8; ++i) {
		float angle = i * (M_PI / 4.0f); // Increment angle by 45 degrees (π/4 radians)
		directions.push_back(Vector3(cos(angle), 0, sin(angle)));
	}

	// Loop through each direction vector in directions
	for (const auto& dir : directions) {
		// Calculate the sphere center using the direction vector and the distance
		Vector3 sphereCenter = center + dir * distance;

		// Translate and scale the model matrix
		Matrix44 m = model;
		m.translate(sphereCenter.x, sphereCenter.y, sphereCenter.z);
		m.scale(sphere_radius, sphere_radius, sphere_radius);

		// Set up material properties and shader uniforms
		material.shader->enable();
		material.shader->setUniform("u_color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
		material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		material.shader->setUniform("u_model", m);

		// Render the sphere mesh
		mesh->render(GL_LINES);

		// Disable the shader
		material.shader->disable();
	}

	/*Matrix44 m = model;
	float sphere_radius = 1.0f;
	material.shader->enable();

	m.translate(0.0f, 2.5f, 0.0f);
	m.scale(sphere_radius, sphere_radius, sphere_radius);

	material.shader->setUniform("u_color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	material.shader->setUniform("u_model", m);

	mesh->render(GL_LINES);

	material.shader->disable();*/
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

	// Maximum speed allowed for the player
	const float max_speed = 20.0f;

	// Calculate the horizontal speed (consider only x and z components)
	float horizontal_speed = sqrt(new_velocity.x * new_velocity.x + new_velocity.z * new_velocity.z);

	if (horizontal_speed < max_speed) {
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
	
	if (in_the_air == this->onFloor) {
		this->dash_cooldown = 0.0f;
	}
	else if (this->onFloor) {
		this->dash_cooldown -= elapsed_time;
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

	//Reducimos velocity mientras no nos movemos (lentamente para que sea más smooth)
	if (move_dir.length() == 0 && this->onFloor) {
		velocity.x -= velocity.x * 10.0f * elapsed_time;
		velocity.z -= velocity.z * 10.0f * elapsed_time;
	}

	float offset = DEG2RAD * 90.0f;

	this->model.setTranslation(position);
	this->model.rotate(camera_yaw-offset, Vector3(0, 1, 0));

	std::cout << "onFloor: " << this->onFloor << std::endl;

	Entity::update(elapsed_time);
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

			//position = collision.colPoint;
			if(!this->ground_pound)
				this->is_dashing = false;

			Vector3 newDir = final_vel.dot(collision.colNormal) * collision.colNormal;
			// If normal is pointing upwards, it means it's a floor collision
			final_vel.x -= newDir.x;
			final_vel.y -= newDir.y;
			final_vel.z -= newDir.z;
			//printf("%f %f %f \n", newDir.x, newDir.y, newDir.z);
			final_vel.x /= 2.0f;
			final_vel.z /= 2.0f;

			//If othere doesn't work
			/*final_vel.x = 0.0f;
			final_vel.z = 0.0f;*/
		}
	}

	if (!WallsCollisions.empty()) {
		for (const sCollisionData& collision : WallsCollisions) {

			Vector3 newDir = final_vel.dot(collision.colNormal) * collision.colNormal;
			// If normal is pointing upwards, it means it's a floor collision
			final_vel.x -= newDir.x;
			final_vel.z -= newDir.z;
			float up_factor = collision.colNormal.dot(Vector3::UP);
			//Check collision with ceiling
			if (up_factor < -0.7f)
				final_vel.y -= newDir.y;
			//printf("%f %f %f \n", newDir.x, newDir.y, newDir.z);
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
		}
	}
	else {
		this->onFloor = false;
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
		printf("%f %f %f", this->model.getTranslation().x, this->model.getTranslation().y, this->model.getTranslation().z);
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
		this->time_for_groundpound = 0.5f;
		position.y += 0.3f;
		//printf("jumping\n");
	}
	//Ground pound
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && !this->onFloor && !this->ground_pound && this->time_for_groundpound < 0.0) {
		this->ground_pound = true;
		this->time_for_groundpound = 0.0;
		velocity.x = 0.0f;
		velocity.z = 0.0f;
		//printf("ground pound\n");
	}
	//Dash
	if (!this->is_dashing && Input::isMousePressed(SDL_BUTTON_LEFT) && this->dash_cooldown <= 0.0f && !this->ground_pound) { //Input::isKeyPressed(SDL_BUTTON_LEFT)) {
		this->is_dashing = true;
		this->dash_timer = 0.2;
		this->dash_cooldown = 1.0;
		//Tenemos que hacer un contador de 1 o 2 segundos que empiece al pulsar el dash y que cuando acabe le reste 5 a la velocidad y devuelva el bool a false
	}
}