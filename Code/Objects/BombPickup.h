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

private:
	MeshComponent* PickupMesh = nullptr;
	CollisionComponent* PickupCollision = nullptr;
};