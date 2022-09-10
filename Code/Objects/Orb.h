#pragma once
#include <Objects/WorldObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>

class Orb : public WorldObject
{
public:
	Orb() : WorldObject(ObjectDescription("Orb", 8)) {}
	void Begin() override;
	void Tick() override;
	void Destroy() override;

private:
	MeshComponent* OrbMesh = nullptr;
	CollisionComponent* OrbCollision = nullptr;
};