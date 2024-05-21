#include "entityCollider.h"

EntityCollider::EntityCollider(Mesh* mesh, int lay) {
	this->layer = lay;
	this->mesh = mesh;
}

bool EntityCollider::is_colliding() {
	if (this->layer && this->layer == SCENARIO) {
		return false;
	}
	//TO DO: Definir altura del player
	float radius = 1;
	Vector3 character_center = this->model.getTranslation() + Vector3(0, 1, 0);
	if (mesh->testSphereCollision(this->model, character_center, radius, col_point, col_normal)) {
		std::cout << "Collision!" << std::endl;
	}

}
