#include "entityui.h"
#include "framework/entities/entity.h"
#include "game/game.h"
#include "framework/camera.h"
#include <string>


EntityUI::EntityUI(Vector2 s, const Material& mat) {
	this->size = s;
	this->mesh = new Mesh();
	mesh->createQuad(0.0, 0.0, s.x, s.y, true);
	this->material = mat;
	this->button_type = UndefinedButton;
}

EntityUI::EntityUI(Vector2 pos, Vector2 s, const Material& mat, eButtonId button_id) {
	this->size = s;
	this->position = pos;
	this->mesh = new Mesh();
	mesh->createQuad(pos.x, pos.y, s.x, s.y, true);
	this->material = mat;
	this->button_type = button_id;


	model.setIdentity();	
	model.translate(pos.x, pos.y, 0.0);
}

void EntityUI::render(Camera* camera) {
	if (!visible) return;

	if (!mesh) return;
	
	if (this->material.shader) {
	}
	else {
		if (material.diffuse) {
			this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
		}
		else {
			this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/color.fs");
		}
	}

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
	material.shader->enable();

	World* world = World::get_instance();
	Matrix44 viewProj = world->camera2D->viewprojection_matrix;

	material.shader->setUniform("u_model", model);
	//material.shader->setUniform("u_camera_position",world->camera2D->eye);
	material.shader->setUniform("u_mesh", mesh);
	material.shader->setUniform("u_viewprojection", viewProj);
	material.shader->setUniform("u_time", time);

	if (material.diffuse) {
		material.shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		material.shader->setUniform("u_texture", material.diffuse, 0);
	}
	else {
		material.shader->setUniform("u_color", material.color);
		material.shader->setUniform("u_texture", Texture::getWhiteTexture(), 0);
	}

	mesh->render(GL_TRIANGLES);

	material.shader->disable();

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->render(camera);
	}
	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
}

void EntityUI::updateUI(float elapsed_time) {
	Vector2 mouse_pos = Input::mouse_position;
	World* world = World::get_instance();

	if (this->button_type != UndefinedButton && world->on_pause) {

		float min_x = this->position.x + 60;
		float max_x = min_x + this->size.x;
		float min_y = this->position.y + 60*button_type;
		float max_y = min_y + this->size.y;

		// Check if our mouse is inside the button
		if (mouse_pos.x > min_x && mouse_pos.x < max_x && mouse_pos.y > min_y && mouse_pos.y < max_y) {	
			if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
				switch (this->button_type) {
				case Restart_level:
					printf(" + Restart level clicked\n");
					world->restart_level = true;
					break;
				case Restart_game:
					printf(" + Restart game clicked\n");
					world->restart_all = true;
					break;
				case Resume:
					printf(" + Resume clicked\n");
					world->resume = true;
					break;
				}
			}
		}
	}
}