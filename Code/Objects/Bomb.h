#pragma once
#include <Objects/WorldObject.h>
#include <Objects/Components/MeshComponent.h>

class Bomb : public WorldObject
{
public:
	Bomb() : WorldObject(ObjectDescription("Bomb", 5)) {}

	void Begin() override;
	void Tick() override;
	void Destroy() override;

protected:
	float DetonationTime = 1.0f;
	MeshComponent* BombMesh = nullptr;
};