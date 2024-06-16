#include "world.h"
#include <iostream> // For input/output streams
#include <fstream>  // For file input/output streams
#include "framework/audio.h"

World* World::instance = nullptr;

World::World() {

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	instance = this;
	root = new Entity();
	player = nullptr;
	loadCubeMap();

	this->current_tutorial = 0;

	//parseScene("data/myscene.scene");
	parseScene("data/level.scene");
	loadPlayer();
	loadAudios();

	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0.0f, width, height, 0, -1.0f, 1.0f);
	loadUI();
	loadGUI();

	this->end_reached = false;
	this->current_check_point = 0;
	this->tutorial_visible = true;
	this->on_pause = false;
	this->restart_all = false;
	this->restart_level = false;
	this->resume = false;

	
}

void World::loadGUI(){
	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	Material restart_game_mat, restart_lev_mat, resume_mat, bg_mat;

	bg_mat.diffuse = Texture::Get("data/textures/gui/bg_pause.png");
	EntityUI* bg = new EntityUI(Vector2(width * 0.25, height * 0.25), Vector2(800, 600), bg_mat);
	gui_icons.push_back(*bg);

	restart_lev_mat.diffuse = Texture::Get("data/textures/gui/restart_level.png");
	this->restart_lev_ui = new EntityUI(Vector2(width * 0.25, height * 0.15), Vector2(540/2, 125/2), restart_lev_mat, Restart_level);
	gui_icons.push_back(*restart_lev_ui);

	restart_game_mat.diffuse = Texture::Get("data/textures/gui/reload_game.png");
	this->restart_game_ui = new EntityUI(Vector2(width * 0.25, height * 0.25), Vector2(540 / 2, 125 / 2), restart_game_mat, Restart_game);
	gui_icons.push_back(*restart_game_ui);

	resume_mat.diffuse = Texture::Get("data/textures/gui/resume.png");
	this->resume_ui = new EntityUI(Vector2(width * 0.25, height * 0.35), Vector2(540 / 2, 125 / 2), resume_mat, Resume);
	gui_icons.push_back(*resume_ui);

}

void World::renderGUI(){
	if (this->on_pause) {
		for (int i = 0; i < gui_icons.size(); i++) {
			gui_icons[i].render(camera2D);
		}
	}
}

void World::loadUI() {
	int width = Game::instance->window_width;
	int height = Game::instance->window_height
;
	Material jump_mat, dash_mat, ground_mat, run_mat , tut_1, tut_2, tut_3, tut_4;

	jump_mat.diffuse = Texture::Get("data/textures/ui/jump_button.png");
	jump_icon = new EntityUI(Vector2(width * 0.4, height * 0.45), Vector2(width*0.05, width * 0.05), jump_mat);

	dash_mat.diffuse = Texture::Get("data/textures/ui/dash_button.png");
	dash_icon = new EntityUI(Vector2(width * 0.43, height * 0.45), Vector2(width * 0.05, width * 0.05), dash_mat);

	ground_mat.diffuse = Texture::Get("data/textures/ui/ground_button.png");
	ground_icon = new EntityUI(Vector2(width * 0.46, height * 0.45), Vector2(width * 0.05, width * 0.05), ground_mat);

	run_mat.diffuse = Texture::Get("data/textures/ui/running.png");
	running_icon = new EntityUI(Vector2(width * 0.05, height * 0.45), Vector2(width * 0.05, width * 0.05), run_mat);

	tut_1.diffuse = Texture::Get("data/textures/ui/tutorial1.png");
	EntityUI* tut1 = new EntityUI(Vector2(width*0.25, 10), Vector2(800, 50), tut_1);
	tutorials.push_back(*tut1);
	tut_2.diffuse = Texture::Get("data/textures/ui/tutorial2.png");
	EntityUI* tut2 = new EntityUI(Vector2(width * 0.25, 10), Vector2(800, 50), tut_2);
	tutorials.push_back(*tut2);
	tut_3.diffuse = Texture::Get("data/textures/ui/tutorial3.png");
	EntityUI* tut3 = new EntityUI(Vector2(width * 0.25, 10), Vector2(800, 50), tut_3);
	tutorials.push_back(*tut3);
	tut_4.diffuse = Texture::Get("data/textures/ui/tutorial4ex.png");
	EntityUI* tut4 = new EntityUI(Vector2(width * 0.25, 10), Vector2(800, 50), tut_4);
	tutorials.push_back(*tut4);
} 

void World::renderUI() {
	if (jump_icon && dash_icon && ground_icon && running_icon) {
		jump_icon->render(camera2D);
		dash_icon->render(camera2D);
		ground_icon->render(camera2D);
		running_icon->render(camera2D);
	}

	if (this->tutorial_visible && tutorial_timer > 0.0f) {
		tutorials[current_tutorial].render(camera2D);
	}
}

void World::addEntity(Entity* entity) {
	entities.push_back(entity);
}

void World::removeEntity(Entity* entity) {
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end()) {
        entities.erase(it);
    }
}


void World::removeAllEntities() {
	entities.clear();
}

bool World::parseScene(const char* filename)
{
	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
		return false;
	}

	std::string scene_info, mesh_name, model_data;
	file >> scene_info; file >> scene_info;
	int mesh_count = 0;

	std::map<std::string, sRenderData> meshes_to_load;

	// Read file line by line and store mesh path and model info in separated variables
	while (file >> mesh_name >> model_data)
	{
		if (mesh_name[0] == '#')
			continue;

		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		// Add model to mesh list (might be instanced!)
		sRenderData& render_data = meshes_to_load[mesh_name];
		render_data.models.push_back(model);
		mesh_count++;
	}

	// Iterate through meshes loaded and create corresponding entities
	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;
		sRenderData& render_data = data.second;

		// No transforms, nothing to do here
		if (render_data.models.empty())
			continue;


		Material mat = render_data.material;
		EntityCollider* new_entity = nullptr;

		size_t tag = data.first.find("@tag");

		if (tag != std::string::npos) {
			Mesh* mesh = Mesh::Get("...");
			// Create a different type of entity
			// new_entity = new ...
		}
		else { //mesh has to have the same name as the entity
			Mesh* mesh = Mesh::Get(mesh_name.c_str());
			new_entity = new EntityCollider(mesh, mat);
		}

		if (!new_entity) {
			continue;
		}

		new_entity->name = data.first;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			if(!new_entity->isInstanced)
				new_entity->material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/pong_color_fog.fs");
			new_entity->isInstanced = true;
			new_entity->models = render_data.models; // Add all instances
		}
		// Create normal entity
		else {
			new_entity->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/pong_color_fog.fs");
			new_entity->model = render_data.models[0];
		}

		new_entity->use_pong = true;
		new_entity->fog = true;

		// Add entity to scene root
		root->addChild(new_entity);
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}

void World::loadPlayer() {
	Texture* texture = Texture::Get("data/Player/Texture.png");
	Shader* shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/pong_texture_fog.fs");
	Mesh* mesh = Mesh::Get("data/Player/Initial_Astronaut.MESH");
	Material material;
	material.diffuse = texture;
	material.shader = shader;
	EntityPlayer* tmp = new EntityPlayer(mesh, material);
	this->player = tmp;
	this->player->use_pong = true;
	this->player->fog = true;
	Vector3 init_position = Vector3(0.0, 10.000000, 0.0);
	this->player->model.setTranslation(init_position);
}

void World::loadCubeMap() {

	Texture* cubemap = new Texture;

	cubemap->loadCubemap("snowBG", {
		"data/textures/snowBG/posx.png",
		"data/textures/snowBG/negx.png",
		"data/textures/snowBG/negy.png",
		"data/textures/snowBG/posy.png",
		"data/textures/snowBG/posz.png",
		"data/textures/snowBG/negz.png"
		});

	Mesh* mesh;
	mesh = Mesh::Get("data/meshes/box.ASE");
	Shader* shader;
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubemap.fs");

	landscape = new EntityMesh(mesh, shader, cubemap, "snowBG");
}

void World::renderAll(Camera* camera) {

	// We render the background image
	glDisable(GL_DEPTH_TEST);
	landscape->render(camera);
	glEnable(GL_DEPTH_TEST);

	root->render(camera);
	player->render(camera);

	renderUI();
	renderGUI();
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
}

void World::updateAll(float delta_time) {
	if (Input::wasKeyPressed(SDL_SCANCODE_P) || this->resume) {
		this->on_pause = !this->on_pause;
		Game::instance->mouse_locked = !Game::instance->mouse_locked;
		SDL_ShowCursor(!Game::instance->mouse_locked);
		SDL_SetRelativeMouseMode((SDL_bool)(Game::instance->mouse_locked));
		if (!this->on_pause) {
			this->resume = false;
			this->restart_level = false;
			this->restart_all = false;
		}

	}
	if (!this->on_pause) {
		player->update(delta_time);
		root->update(delta_time);
		check_chekpoints();
		if (this->tutorial_timer > 0.0f) {
			tutorial_timer -= delta_time;
		}
	}
}

void World::updateCubemap(Camera* camera) {
	this->landscape->model.setTranslation(camera->eye);
}

void World::setCamerayaw(Camera* camera) {
	this->camera_yaw = camera->getYaw();
}


void World::renderEntities(Camera* camera) {
	// Using entities container

	for (int i = 0; i < entities.size(); ++i) {
		Entity* ent = entities[i];

		if (ent->getEntityType() == eEntityType::MESH) {

			EntityMesh* pent = (EntityMesh*)ent;
			// Compute bounding sphere center 
			// in world coords
			Vector3 sphere_center = pent->model *
				pent->mesh->box.center;
			float sphere_radius = pent->mesh->radius;

			// Discard objects whose bounding sphere 
			// is not inside the camera frustum
			if (camera->testSphereInFrustum(sphere_center,
				sphere_radius) == false)
				continue;

			// Render object if inside frustum
			pent->render(camera);
		}
		else if (ent->getEntityType() == eEntityType::PLAYER) {

			//Do rendering for player entity
		}

	}
}

World* World::get_instance() {
    if (instance == nullptr) {
        instance = new World();
    }
    return instance;
}

void World::check_chekpoints() {
	Vector3 position = this->player->model.getTranslation();
		if ((position.x > 522 && position.y >28 && this->current_check_point == 1) || (position.x > 242 && position.y > -4 && this->current_check_point == 0) || (position.x > 870 && position.y > -110 && this->current_check_point == 2) || (position.x > 1411 && position.y > -70 && this->current_check_point == 3) || (position.x > 1896.71 && position.y > -77.92f && this->current_check_point == 4)) {
			this->current_check_point++;
		}
		if (position.y <= -170) {
			this->player->model.setTranslation(this->checkpoints[this->current_check_point]);
			this->player->velocity = vec3(0.0f, 2.0f, 0.0f);
			this->player->PlayAnimation("Idle", true, 0.0f);
			Audio::Play("data/sounds/ReviveSound.wav");
		}
		if ((current_tutorial == 0 && position.x > 100) || (current_tutorial == 1 && position.x > 520) || (current_tutorial == 2 && position.x > 930)) {
			this->tutorial_visible = true;
			current_tutorial++;
			tutorial_timer = 10.0f;
		}
		if ((position.x > 2060 && position.y > -65 && position.z < -100) || position.x < -50) {
			this->end_reached = true;
		}
}

void World::loadAudios(){
	Audio::Init();
	Audio::Get("data/sounds/Bgm.wav");
	Audio::Get("data/sounds/ReviveSound.wav");
	Audio::Get("data/sounds/Jump.wav");
	Audio::Get("data/sounds/GroundPound.wav");
	Audio::Get("data/sounds/Dash.wav");
	Audio::Get("data/sounds/OnGround.wav");
	
	//Audio::Play("data/sounds/bgm2.wav");
}
