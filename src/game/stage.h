#pragma once
#include "framework/input.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityMesh.h"
#include "framework/camera.h"
#include "game.h"
#include <string>

enum {
	INTRO_STAGE,
	//CONTROLS_STAGE,
	//MENU,
	PLAY_STAGE,
	GAME_OVER_STAGE,
	STAGES_SIZE
};


class Stage
{
	int type = INTRO_STAGE;

public:

	Camera* camera;
	float mouse_speed = 100.0f;

	Stage() { };
	std::vector<Entity*> entities;

	virtual void onEnter() {};
	virtual void onExit() {};
	void addEntity(Entity* entity);
	void removeEntity(Entity* entity);
	virtual void render( void ) {};
	virtual void update(float second_elapsed) {};
};

class IntroStage : public Stage
{
	int type = INTRO_STAGE;

public:

	float angle;
	EntityMesh* cube;

	IntroStage() {};
	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void render( void ) override;
	virtual void update(float second_elapsed) override;
};

class PlayStage : public Stage
{
	int type = PLAY_STAGE;

public:

	PlayStage() {};
	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void render(void) override;
	virtual void update(float second_elapsed) override;
};

class GameOverStage : public Stage
{
	int type = GAME_OVER_STAGE;

public:

	GameOverStage() {};
	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void render(void) override;
	virtual void update(float second_elapsed) override;

};

