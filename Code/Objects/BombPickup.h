#pragma once
#include <Objects/WorldObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>

class BombPickup : public WorldObject
{
public:
	BombPickup() : WorldObject(ObjectDescription("Bomb Pickup", 7)) {}
	void Begin() override;
	void Tick() override;
	void Destroy() override;

	float Amount = 5.f;
private:
	MeshComponent* PickupMesh = nullptr;
	CollisionComponent* PickupCollision = nullptr;
};