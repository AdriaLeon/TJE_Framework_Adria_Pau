#include "stage.h"


void IntroStage::onEnter() {

	angle = 0;

    // Create our camera
    camera = new Camera();
    camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
    camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	world = new World();

    //We set the first person camera parameters
    Game::instance->mouse_locked = true;
    SDL_ShowCursor(!Game::instance->mouse_locked);
    SDL_SetRelativeMouseMode((SDL_bool)(Game::instance->mouse_locked));
    camera->default_camera = false;
    camera->first_person_mode_front = true;

	//Method 1
    // Load one texture using the Texture Manager
	/*Texture* texture;
    texture = Texture::Get("data/textures/texture.tga");

    // Example of shader loading using the shaders manager
	Shader* shader;
    shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

    Mesh* mesh;
    // Example of loading Mesh from Mesh Manager
    mesh = Mesh::Get("data/meshes/Ghost Character.ASE");

    cube = new EntityMesh(mesh, shader, texture, "Char");

	//Method 2
	cube = new EntityMesh((char*)"data/meshes/box.ASE", (char*)"data/shaders/basic.vs", (char*)"data/shaders/texture.fs", (char*)"data/textures/texture.tga", "Cube");

	cube->render(camera);
	*/
}

void IntroStage::onExit() {
	world->removeAllEntities();
};

void IntroStage::render( void ) {

	// Set the camera as default
	camera->enable();

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
;
	 
	world->renderAll(camera);

	// Draw the floor grid
	//drawGrid();
}

void IntroStage::update(float second_elapsed) {

    float speed = second_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

    world->updateAll(second_elapsed);

    // Example
    angle += (float)second_elapsed * 10.0f;

    static float yaw = 0.0f;
    static float pitch = 0.0f;

    // Mouse input to rotate the cam
    if (Input::isMousePressed(SDL_BUTTON_LEFT) || Game::instance->mouse_locked) //is left button pressed?
    {
        //TODO: Ajustar la sensibilidad 
        yaw -= Input::mouse_delta.x * speed;
        pitch -= Input::mouse_delta.y * speed;
        // Limit pitch to avoid flipping
        pitch = clamp(pitch, -1.5f, 1.0f);

        
        camera->rotate(yaw, Vector3(0.0f, 1.0f, 0.0f));
        camera->rotate(pitch, camera->getLocalVector(Vector3(1.0f, 0.0f, 0.0f)));
    }


    // Toggle between first person and third person view when 'C' is pressed
    if (Input::wasKeyPressed(SDL_SCANCODE_C))
    {
        Game::instance->mouse_locked = !Game::instance->mouse_locked;
        SDL_ShowCursor(!Game::instance->mouse_locked);
        SDL_SetRelativeMouseMode((SDL_bool)(Game::instance->mouse_locked));
        camera->default_camera = !camera->default_camera;
        if (camera->default_camera == true) {
            //Set the default position of the camera
            camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f));
            camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f);
        }
    }

    if (Input::wasKeyPressed(SDL_SCANCODE_X))
    {
        camera->first_person_mode_front = !camera->first_person_mode_front;
    }

    if (camera->default_camera)
    {
        if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
        if (Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
        if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
        if (Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
        if (Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
    }
    else
    {
        // First and Third person view
        Matrix44 pitch_matrix;
        pitch_matrix.setRotation(pitch, Vector3(-1.0f, 0.0f, 0.0f)); // Pitch rotation

        Matrix44 yaw_matrix;
        yaw_matrix.setRotation(yaw, Vector3(0.0f, 1.0f, 0.0f)); // Yaw rotation

        // Combine pitch and yaw rotations
        Matrix44 final_rotation = pitch_matrix * yaw_matrix;

        // Update camera position to follow the player
        Vector3 front = final_rotation.frontVector().normalize();

        Vector3 eye;
        // First person view
        if (camera->first_person_mode_front) {
            eye = world->player->model.getTranslation() + Vector3(0.0f, 3.5f, 0.0f) + front; // Adjust height to player's eye level
        }
        else { // Third person view
            eye = world->player->model.getTranslation() + Vector3(0.0f, 4.5f, 0.0f) - 10 * front;
        }


        float smoothingFactor = 0.5f; // Adjust this factor for smoother transitions (between 0 and 1)

        // Interpolate current position towards the target position
        eye = lerp(eye, camera->eye, smoothingFactor * second_elapsed);

        //Do ray tracing in case tha camera in third person enters a wall.
        Vector3 center = eye + front;

        center = lerp(center, camera->center, smoothingFactor * second_elapsed);

        // Update the camera position
        camera->lookAt(eye, center, Vector3(0, 1, 0));
    }

    world->setCamerayaw(camera);
    world->updateCubemap(camera);
}

void updateCameraPosition(Camera* camera, Vector3 targetEye, Vector3 targetCenter, float deltaTime) {
    static Vector3 currentEye = targetEye; // Initialize with the target position
    static Vector3 currentCenter = targetCenter; // Initialize with the target position

    float smoothingFactor = 0.1f; // Adjust this factor for smoother transitions (between 0 and 1)

    // Interpolate current position towards the target position
    currentEye = lerp(currentEye, targetEye, smoothingFactor * deltaTime);
    currentCenter = lerp(currentCenter, targetCenter, smoothingFactor * deltaTime);

    // Update the camera position
    camera->lookAt(currentEye, currentCenter, Vector3(0, 1, 0));
}

void PlayStage::onEnter() {};
void PlayStage::onExit() {};
void PlayStage::render(void) {}; 
void PlayStage::update(float second_elapsed) {};

void GameOverStage::onEnter() {};
void GameOverStage::onExit() {};
void GameOverStage::render(void) {};
void GameOverStage::update(float second_elapsed) {};