#pragma once
#include "Math/Vector.h"
#include "glm/glm.hpp"

struct Material
{
	Vector3 Diffuse = Vector3(0.5f, 0.5f, 0.5f);
	float SpecularSize = 2;
	float SpecularStrength = 0;
	Vector3 Emissive = 0.f;
};