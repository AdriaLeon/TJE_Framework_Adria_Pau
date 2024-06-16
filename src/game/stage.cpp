#include "stage.h"
#include "framework/includes.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"
#include "framework/entities/entityMesh.h"
#include "framework/entities/entityui.h"
#include "framework/camera.h"
#include "framework/audio.h"
#include "game.h"
#include <string>
float time_to_wait = 7.0f;


void EndStage::onEnter() {
    int width = Game::instance->window_width;
    int height = Game::instance->window_height;
    Audio::Init();
    Audio::Get("data/sounds/Title.wav");
    camera2D = new Camera();
    camera2D->view_matrix.setIdentity();
    camera2D->setOrthographic(0.0f, width, height, 0, -1.0f, 1.0f);
    loadIMG();
    this->timer = time_to_wait;
    this->current_img = 0;
    this->TitleBG = 0;
}

void EndStage::loadIMG(){
    int width = Game::instance->window_width;
    int height = Game::instance->window_height;

    Material img1_mat, img2_mat, img3_mat, img4_mat;
    img1_mat.diffuse = Texture::Get("data/textures/end/end1.png");
    EntityUI* img1 = new EntityUI(Vector2(width * 0.25, height * 0.25), Vector2(800, 600), img1_mat);
    images.push_back(img1);
    img2_mat.diffuse = Texture::Get("data/textures/end/end2.png");
    EntityUI* img2 = new EntityUI(Vector2(width * 0.25, height * 0.25), Vector2(800, 600), img2_mat);
    images.push_back(img2);
    img3_mat.diffuse = Texture::Get("data/textures/end/end3.png");
    EntityUI* img3 = new EntityUI(Vector2(width * 0.25, height * 0.25), Vector2(800, 600), img3_mat);
    images.push_back(img3);
    img4_mat.diffuse = Texture::Get("data/textures/intro/intro5.png");
    EntityUI* img4 = new EntityUI(Vector2(width * 0.25, height * 0.25), Vector2(800, 600), img4_mat);
    images.push_back(img4);
}
void EndStage::onExit(){
    this->images.clear();
    Audio::Stop(this->TitleBG);
}
void EndStage::rederIMG(){}
void EndStage::update(float seconds_elapsed){
    if (this->timer > 0.0f) {
        this->timer -= seconds_elapsed;
    }
    else if (this->current_img < this->images.size() - 1) {
        current_img++;
        this->timer = time_to_wait;
    }
    if (this->TitleBG == 0) {
        TitleBG = Audio::Play("data/sounds/Title.wav", 0.2, BASS_SAMPLE_LOOP);
    }

    if ((Input::wasKeyPressed(SDL_SCANCODE_SPACE) && this->current_img == this->images.size() - 1) || Input::wasKeyPressed(SDL_SCANCODE_Q)) {
        Game::instance->goToStage(LEVEL_STAGE);
    }
}
void EndStage::render(){
    images[this->current_img]->render(camera2D);
}

void TitleStage::onEnter() {
    int width = Game::instance->window_width;
    int height = Game::instance->window_height;
    Audio::Init();
    Audio::Get("data/sounds/Title.wav");
    camera2D = new Camera();
    camera2D->view_matrix.setIdentity();
    camera2D->setOrthographic(0.0f, width, height, 0, -1.0f, 1.0f);
    loadIMG();
    this->timer = time_to_wait + 4.0f; //First one has extra time
    this->TitleBG = 0;
    this->current_img = 0;
   if (this->TitleBG == 0) {
        TitleBG = Audio::Play("data/sounds/Title.wav", 0.2, BASS_SAMPLE_LOOP);
    }
}

void TitleStage::loadIMG() {
    int width = Game::instance->window_width;
    int height = Game::instance->window_height;

    Material img1_mat, img2_mat, img3_mat, img4_mat, img5_mat;
    img1_mat.diffuse = Texture::Get("data/textures/intro/intro1.png");
    EntityUI* img1 = new EntityUI(Vector2(width*0.25, height*0.25), Vector2(800, 600), img1_mat);
    images.push_back(img1);
    img2_mat.diffuse = Texture::Get("data/textures/intro/intro2.png");
    EntityUI* img2 = new EntityUI(Vector2(width * 0.25, height * 0.25), Vector2(800, 600), img2_mat);
    images.push_back(img2);
    img3_mat.diffuse = Texture::Get("data/textures/intro/intro3.png");
    EntityUI* img3 = new EntityUI(Vector2(width * 0.25, height * 0.25), Vector2(800, 600), img3_mat);
    images.push_back(img3);
    img4_mat.diffuse = Texture::Get("data/textures/intro/intro4.png");
    EntityUI* img4 = new EntityUI(Vector2(width * 0.25, height * 0.25), Vector2(800, 600), img4_mat);
    images.push_back(img4);
    img5_mat.diffuse = Texture::Get("data/textures/intro/intro5.png");
    EntityUI* img5 = new EntityUI(Vector2(width * 0.25, height * 0.25), Vector2(800, 600), img5_mat);
    images.push_back(img5);
}

void TitleStage::update(float seconds_elapsed) {
    if (this->timer > 0.0f) {
        this->timer -= seconds_elapsed;
    }
    else if(this->current_img < this->images.size() - 1){
        current_img++;
        this->timer = time_to_wait;
    }
    if ((Input::wasKeyPressed(SDL_SCANCODE_SPACE) && this->current_img == this->images.size() - 1) || Input::wasKeyPressed(SDL_SCANCODE_Q)) {
        Game::instance->goToStage(LEVEL_STAGE);;
    }

}

void TitleStage::render(void) {
    images[this->current_img]->render(camera2D);
}

void TitleStage::onExit() {
    images.clear();
    Audio::Stop(TitleBG);
}

void LevelStage::onEnter() {

	angle = 0;
    this->channelBG = 0;
    // Create our camera
    camera = new Camera();
    camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
    camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective
    this->channelBG = Audio::Play("data/sounds/Bgm.wav", 0.2, BASS_SAMPLE_LOOP);

	world = new World();


    //We set the first person camera parameters
    Game::instance->mouse_locked = true;
    SDL_ShowCursor(!Game::instance->mouse_locked);
    SDL_SetRelativeMouseMode((SDL_bool)(Game::instance->mouse_locked));
    camera->default_camera = false;
    camera->first_person_mode_front = true;
}

void LevelStage::onExit() {
    Audio::Stop(this->channelBG);
	world->removeAllEntities();
};

void LevelStage::render( void ) {

	// Set the camera as default
	camera->enable();

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
;
	 
	world->renderAll(camera);

}

void LevelStage::update(float second_elapsed) {

    if (world->end_reached) {
        Game::instance->goToStage(END_STAGE);
    }
    if(world->restart_all){
        Game::instance->goToStage(TITLE_STAGE);
    }
    if (world->restart_level) {
        Game::instance->goToStage(LEVEL_STAGE);
    }

    float speed = second_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

    world->updateAll(second_elapsed);

    if (world->on_pause) {
        world->restart_game_ui->updateUI(second_elapsed);
        world->restart_lev_ui->updateUI(second_elapsed);
        world->resume_ui->updateUI(second_elapsed);
    }
    else {
        angle += (float)second_elapsed * 10.0f;

        static float yaw = 0.0f;
        static float pitch = 0.0f;

        // Mouse input to rotate the cam
        if (Input::isMousePressed(SDL_BUTTON_LEFT) || Game::instance->mouse_locked) //is left button pressed?
        {
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

