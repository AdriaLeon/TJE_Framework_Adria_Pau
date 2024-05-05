#include "world.h"
#include <iostream> // For input/output streams
#include <fstream>  // For file input/output streams

World* World::instance = NULL;

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
bool World::parseScene(const char* filename, Entity* root)
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
			new_entity = new EntityMesh(mesh, &mat);
		}

		if (!new_entity) {
			continue;
		}

		new_entity->name = data.first;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			new_entity->isInstanced = true;
			new_entity->models = render_data.models; // Add all instances
		}
		// Create normal entity
		else {
			new_entity->model = render_data.models[0];
		}

		// Add entity to scene root
		root->addChild(new_entity);
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
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
