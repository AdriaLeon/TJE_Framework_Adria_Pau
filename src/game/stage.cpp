#include "stage.h"

void Stage::addEntity(Entity* entity) {
    entities.push_back(entity);
}

void Stage::removeEntity(Entity* entity) {
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end()) {
        entities.erase(it);
    }
}

void IntroStage::onEnter() {

	angle = 0;

    // Create our camera
    camera = new Camera();
    camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
    camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

    // Load one texture using the Texture Manager
	Texture* texture;
    texture = Texture::Get("data/textures/texture.tga");

    // Example of shader loading using the shaders manager
	Shader* shader;
    shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

    Mesh* mesh;
    // Example of loading Mesh from Mesh Manager
    mesh = Mesh::Get("data/meshes/box.ASE");

    cube = new EntityMesh(mesh, shader, texture, "Cube");


}

void IntroStage::onExit() {};

void IntroStage::render( void ) {
	// Set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the camera as default
	camera->enable();

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Create model matrix for cube
	Matrix44 m;
	m.rotate(angle * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));
	cube->model = m;
	cube->render(camera);

	// Draw the floor grid
	drawGrid();

	// Render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	// Swap between front buffer and back buffer
	SDL_GL_SwapWindow(Game::instance->window);
}

void IntroStage::update(float second_elapsed) {

	float speed = second_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	// Example
	angle += (float)second_elapsed * 10.0f;

	// Mouse input to rotate the cam
	if (Input::isMousePressed(SDL_BUTTON_LEFT) || Game::instance->mouse_locked) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	// Async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
}

void PlayStage::onEnter() {};
void PlayStage::onExit() {};
void PlayStage::render(void) {}; 
void PlayStage::update(float second_elapsed) {};

void GameOverStage::onEnter() {};
void GameOverStage::onExit() {};
void GameOverStage::render(void) {};
void GameOverStage::update(float second_elapsed) {};