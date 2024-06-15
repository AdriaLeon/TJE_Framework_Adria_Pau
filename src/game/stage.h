#pragma once
#include "framework/input.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityMesh.h"
#include "framework/camera.h"
#include "game.h"
#include "world.h"
#include <string>

enum {
	TITLE_STAGE,
	LEVEL_STAGE,
	PLAY_STAGE,
	GAME_OVER_STAGE,
	STAGES_SIZE
};

// Forward declaration of World class
class World;
class EntityUI;
class Camera;

class Stage
{
	int type = TITLE_STAGE;

public:
	World* world;
	Camera* camera;

	float mouse_speed = 1.0f;

	Stage() { };

	virtual void onEnter() {};
	virtual void onExit() {};
	virtual void render( void ) {};
	virtual void update(float second_elapsed) {};
};

class TitleStage : public Stage {
	int type = TITLE_STAGE;

public:
	std::vector<EntityUI*> images;
	Camera* camera2D;
	TitleStage() {};
	int current_img = 0;
	float timer;
	HCHANNEL TitleBG = 0;
	virtual void onEnter() override;
	virtual void onExit() override;
	void loadIMG();
	void rederIMG();

	virtual void render(void) override;
	virtual void update(float second_elapsed) override;
};

class LevelStage : public Stage
{
	int type = LEVEL_STAGE;

public:

	float angle;
	EntityMesh* cube;

	LevelStage() {};
	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void render( void ) override;
	virtual void update(float second_elapsed) override;
};

