#pragma once
#include "framework/input.h"
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


	Stage() { };

	virtual void onEnter() {};
	virtual void onExit() {};

	//virtual void render(Image* framebuffer) {};
	virtual void update(float second_elapsed) {};
};

class IntroStage : public Stage
{
	int type = INTRO_STAGE;

public:

	virtual void onEnter() override;
	virtual void onExit() override;

	//virtual void render(Image* framebuffer) override;
	virtual void update(float second_elapsed) override;
};

class PlayStage : public Stage
{
	int type = PLAY_STAGE;

public:

	virtual void onEnter() override;
	virtual void onExit() override;

	//virtual void render(Image* framebuffer) override;
	virtual void update(float second_elapsed) override;
};

class Game_Over_Stage : public Stage
{
	int type = GAME_OVER_STAGE;

public:

	virtual void onEnter() override;
	virtual void onExit() override;

	//virtual void render(Image* framebuffer) override;
	virtual void update(float second_elapsed) override;

};

