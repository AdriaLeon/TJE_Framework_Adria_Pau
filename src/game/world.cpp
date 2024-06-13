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

	//parseScene("data/myscene.scene");
	parseScene("data/level.scene");
	loadPlayer();
	loadAudios();
	channelBG = Audio::Play("data/sounds/Bgm.wav", 0.3, BASS_SAMPLE_LOOP);
	camera2D = new Camera();
	camera2D->lookAt(Vector3(0.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
	camera2D->setPerspective(70.0f, width / height, 0.1f, 100.0f);
	loadUI();

	
}

void World::loadUI() {
	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	Material jump_mat;
	jump_mat.diffuse = Texture::Get("data/textures/ui/hombre-saltando.png");
	bg = new EntityUI(Vector2(width * 0.5, height * 0.5), Vector2(width, height), jump_mat);
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

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	if (bg) {
		bg->render(camera2D);
	}
	*/
}

void World::updateAll(float delta_time) {
	player->update(delta_time);
	root->update(delta_time);
	check_chekpoints();
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
