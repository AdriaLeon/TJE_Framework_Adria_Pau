#include "world.h"
#include <iostream> // For input/output streams
#include <fstream>  // For file input/output streams

World* World::instance = nullptr;

World::World() {

	instance = this;
	root = new Entity();
	player = nullptr;
	loadCubeMap();

	parseScene("data/myscene.scene");
	loadPlayer();
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

//Revisar que fa i adaptar-lo al nostre codi, codi de parseScene proporcionat a l'aula global amb alguna petita modificació
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
		EntityMesh* new_entity = nullptr;

		size_t tag = data.first.find("@tag");

		if (tag != std::string::npos) {
			Mesh* mesh = Mesh::Get("...");
			// Create a different type of entity
			// new_entity = new ...
		}
		else { //mesh has to have the same name as the entity
			Mesh* mesh = Mesh::Get(mesh_name.c_str());
			new_entity = new EntityMesh(mesh, mat);
		}

		if (!new_entity) {
			continue;
		}

		new_entity->name = data.first;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			if(!new_entity->isInstanced)
				new_entity->material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/color.fs");
			new_entity->isInstanced = true;
			new_entity->models = render_data.models; // Add all instances
		}
		// Create normal entity
		else {
			new_entity->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/color.fs");
			new_entity->model = render_data.models[0];
		}

		// Add entity to scene root
		root->addChild(new_entity);
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}

void World::loadPlayer() {
	Texture* texture = Texture::Get("data/Player/Alien.mtl");
	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/color.fs");
	Mesh* mesh = Mesh::Get("data/Player/Alien.obj");
	Material material;
	material.diffuse = texture;
	material.shader = shader;
	EntityPlayer* tmp = new EntityPlayer(mesh, material);
	this->player = tmp;
	Vector3 init_position = Vector3(-41.927399, 0.000000, -282.417572);
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
}

void World::updateAll(float delta_time) {
	player->update(delta_time);
	root->update(delta_time);
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

bool World::check_player_collisions(Vector3& target_pos, std::vector<sCollisionData> collisions) {
	for (Entity* ent : root->children) {
		EntityMesh* e = (EntityMesh*)ent;

		Vector3 ray_start = target_pos + Vector3(0.0, this->player->height / 2, 0.0);
		Vector3 ray_dir = Vector3(0.0, -1.0, 0.0);
		float max_ray_dist = this->player->height/2;
		Vector3 colPoint, colNormal;
		Matrix44 model = e->model;
		Mesh* mesh = e->mesh;

		// Floor collisions
		if (mesh->testRayCollision(model, ray_start, ray_dir, colPoint, colNormal, max_ray_dist, true)) {
			this->player->onFloor = true;
			printf("on floor\n");
		}
		else {
			this->player->onFloor = false;
		}

		// Wall collision
		Vector3 center_sphere = target_pos + Vector3(0.0, this->player->height / 2, 0.0);
		float sphereRadius = 0.025f;
		model = e->model;
		mesh = e->mesh;

		if (mesh->testSphereCollision(model, center_sphere, sphereRadius, colPoint, colNormal)) {
			collisions.push_back({ colPoint, colNormal.normalize() });
			printf("on wall\n");
		}
	}
	return !collisions.empty();
}

/*Class EntityPlayer{

	Collisiones
		//////collisions with world
		#Get instances from world
		sphere_radius
		sphere_ground_radious
		player_height

		Shader, Mesh #get them
		Matrix44 m = model;

		shader->enable();

		//first spher
		{
			m.translate(0,player_height,0);
			m.scale(sphere_radious, sphere_ground_radious);

			....
		}

		//second sphere
		{
			m = model;
			m.translate(0,player_height,0);
			m.scale(sphere_radious, sphere_ground_radious);

			....
		}

		//Check collision with floor then with wall and last if plater its on the floor
		if(mesh->(testSphereCollision(m, floor_sphere_center, sphere_radious, collision_ point, collision_normal)
			collision.push_back({collision_point, collision_normal.normalize(), floor....)


		///////sends a ray whith the mouse and check where it collisions
		Needs an EntityCollider class
		eCollisionFilter type of colliders (do an enum)

		for (entities)
			get collider entities if not collider skip

			Vector3 col_point;  //te lo rellena la funcion
			Vector3 col_normal;  //te lo rellena la funcion
			if(collider->mesh->testRayCollision(collider->model, ray_origin, ray_direction,col_point,col_normal))  //tambien se pude usar max_dist (6 param para saber si esta tocando el suelo distancia entre suelo i centro de personaje)
				collisions.push_back(col_point);
		}

		//Generate entities

		for(auto& col_point : colisions)
		{
			Mesh* mesh = Mesh::Get()
			EntityMesh* new_entity = new EntityMesh(mesh, material);
			new:entity->model.setTranslation(col_point);
			World::get:instance()->addEntity(new_entity);

		
}*/

/*getCollision(....) {
	if (!(layer & filter)) { Check if collitions needs to be checked
		return;
	}

*/
