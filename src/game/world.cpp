#include "world.h"
#include <iostream> // For input/output streams
#include <fstream>  // For file input/output streams

World* World::instance = nullptr;

World::World() {

	instance = this;
	root = new Entity();

	parseScene("data/myscene.scene");
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

void World::renderAll(Camera* camera) {
	root->render(camera);
}

void World::updateAll(float delta_time) {
	root->update(delta_time);
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




///Update

/*
camera_yaw -= input:: mouse.delta.x * elap_second
camera_pitch -= input:: mouse.delta.y * elap_second

if(first_person){
	eye = player->model.getTranslation() + Vector(0,1,0) * obit_dist;
	center = eye + front;
else{ tercera persona){
	eye = player->model.getTranslation() - Vector(0,1,0) + front * obit_dist;
	center = player->model.getTranslation() + Vector(1,1,0);
*/




/*Class EntityPlayer{
	Movimiento
		movement lo mismo que en 2d pero en 3 dimensiones

		Matrix44 wYaw;
		wYaw.setRotation(camera_yaw, Vectro(0,1,0));

		Vector3 front = wYaw.frontVector;
		Vectro3 right = WYaw.rightVector;

		position += velocity * seconds_elapsed;

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