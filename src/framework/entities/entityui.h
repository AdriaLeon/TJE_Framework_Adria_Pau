#pragma once
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "entity.h";
#include "graphics/material.h"
#include "framework/audio.h"
#include "entityMesh.h"

enum eButtonId {
	UndefinedButton,
	OptionButton,
	ExitButton,
	Background
};

class EntityUI : public EntityMesh {
public:
	EntityUI(Vector2 size, const Material& material);
	EntityUI(Vector2 pos, Vector2 size, const Material& material, eButtonId button_id = UndefinedButton);
	~EntityUI() {};

	Vector2 position;
	Vector2 size;
	bool visible = true;
	float mask = 1.0f;
	eButtonId button_type;

	void render(Camera* camera);

private:
};
