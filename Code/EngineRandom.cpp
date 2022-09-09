#include "EngineRandom.h"
#include <random>

int Random::GetRandomNumber(int Min, int Max)
{
	int Range = std::abs(Max - Min);
	int RandomNumber = std::rand() % Range;
	return RandomNumber + Min;
}

float Random::GetRandomNumber(float Min, float Max)
{
	float Range = std::abs(Max - Min);
	float RandomNumber = std::fmod(std::rand(), Range);
	return RandomNumber + Min;
}
