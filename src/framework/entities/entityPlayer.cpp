
#include "entityPlayer.h"
#include "framework/input.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityMesh.h"
#include "framework/camera.h"
#include "game/game.h"
#include "game/world.h"
#include <string>

EntityPlayer::EntityPlayer(Mesh* mesh, Material material){
	this->mesh = mesh;
	this->onFloor = TRUE;
	this->material = material;
	this->walkSpeed = 0.0f;
}

void EntityPlayer::render(Camera* camera) {

	if (!mesh)		return;

	if (!material.shader) {
		material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
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

	mesh->render(GL_TRIANGLES);
	// Disable shader after finishing rendering
	material.shader->disable();
};

void EntityPlayer::update(float elapsed_time) {

	float camera_yaw = World::get_instance()->camera_yaw;

	Matrix44 mYaw;
	mYaw.setRotation(camera_yaw, Vector3(0, 1, 0));

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
	float speed_mult = this->walkSpeed;
	this->model.setTranslation(position + move_dir * speed_mult);

	Entity::update(elapsed_time);
}

void EntityPlayer::setMaterial(Material material) {
	this->material = material;
}