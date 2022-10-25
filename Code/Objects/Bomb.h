#pragma once
#include <Objects/WorldObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/PointLightComponent.h>
#include <GENERATED/GENERATED_Bomb.h>

class Bomb : public WorldObject
{
public:
	BOMB_GENERATED()

	void Begin() override;
	void Tick() override;
	void Destroy() override;

protected:
	bool PlayedSound = false;
	float DetonationTime = 1.0f;
	MeshComponent* BombMesh = nullptr;
	MeshComponent* ExplosionMesh = nullptr;
	PointLightComponent* PointLight = nullptr;
};