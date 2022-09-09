#pragma once
#include "Math/Vector.h"
#include "glm/glm.hpp"
struct Vertex
{
	glm::vec3 Position = glm::vec3(0.f);
	float U = 0.5f;
	float V = 0.5f;
	float ColorR = 1;
	float ColorG = 0.5f;
	float ColorB = 0.25f;
	float ColorA = 1;
	glm::vec3 Normal = glm::vec3(0.f, 1.f, 0.f);
};