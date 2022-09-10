#pragma once
#include <Objects/WorldObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>

class WallObject : public WorldObject
{
public:
	WallObject() : WorldObject(ObjectDescription("Wall Object", 6)) {}

	void Begin() override;
	void Tick() override;
	void Destroy() override;
	void OnPropertySet() override;

	int ContainsOrb = 0;

private:
	std::string MeshName = "BreakableCube";
	MeshComponent* WallMesh = nullptr;
	CollisionComponent* WallCollision = nullptr;
};