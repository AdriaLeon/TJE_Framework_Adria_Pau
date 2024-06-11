#pragma once

#include "framework/includes.h"
#include "framework/framework.h"

class Shader;
class Texture;

class Material {
public:
	Shader* shader = nullptr;
	Vector4 color = Vector4(1.f);
	Texture* diffuse = nullptr;
	vec3 kd = vec3(0.6, 0.6, 0.6);
	vec3 ka = vec3(0.8, 0.8, 0.8);
	vec3 ks = vec3(0.8, 0.8, 0.8);
};

struct sRenderData {
	Material material;
	std::vector<Matrix44> models;
};