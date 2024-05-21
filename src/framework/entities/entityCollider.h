#include "entityMesh.h"

//TODO: Remove this class

enum {
	NONE = 0,
	FLOOR = 1 << 0,
	WALL = 1 << 1,
	PLAYER = 1 << 2,
	// Both WALL and FLOOR are considered SCENARIO
	// using OR operator
	SCENARIO = WALL | FLOOR,
	ALL = 0xFF
};

class EntityCollider : public EntityMesh {
public:
	bool is_dynamic = false;
	int layer = NONE;
	Vector3 col_point;
	Vector3 col_normal;
	
	EntityCollider() {};
	EntityCollider(Mesh* mesh, int lay);
	bool is_colliding();
};
