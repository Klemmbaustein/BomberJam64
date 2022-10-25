#pragma once
#include <Objects/WorldObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>
#include <GENERATED/GENERATED_BombPickup.h>

class BombPickup : public WorldObject
{
public:
	BOMBPICKUP_GENERATED()
	void Begin() override;
	void Tick() override;
	void Destroy() override;

	float Amount = 5.f;
private:
	MeshComponent* PickupMesh = nullptr;
	CollisionComponent* PickupCollision = nullptr;
};