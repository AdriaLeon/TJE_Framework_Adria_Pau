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

	if (this->button_type != UndefinedButton && mouse_pos.x > (position.x - size.x * 0.5) && mouse_pos.x < (position.x + size.x * 0.5) && mouse_pos.y >(position.y - size.y * 0.5) && mouse_pos.y < (position.y + size.y * 0.5)) {
		/*
		if (Input::isMousePressed(SDL_BUTTON_LEFT) && !stage->mouse_clicked) {
			stage->mouse_clicked = true;
			switch (button_id) {
			case PlayButton:
				stage->play_button_pressed = true;
				break;
			case OptionsButton:
				stage->options = true;
				break;
			case ExitButton:
				stage->options = false;
				break;
			case KeyWalk:
				stage->selected_keybind = StageManager::WALK;
				stage->keybinds[stage->selected_keybind] = true;
				break;
			case KeyJump:
				stage->selected_keybind = StageManager::JUMP;
				stage->keybinds[stage->selected_keybind] = true;
				break;
			case KeyDash:
				stage->selected_keybind = StageManager::DASH;
				stage->keybinds[stage->selected_keybind] = true;
				break;
			case KeyShoot:
				stage->selected_keybind = StageManager::SHOOT;
				stage->keybinds[stage->selected_keybind] = true;
				break;
			case KeyAuto:
				stage->selected_keybind = StageManager::AUTO;
				stage->keybinds[stage->selected_keybind] = true;
				break;
			}
			*/
	}
}