#pragma once
#include <Objects/Objects.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>

class OrbGate : public WorldObject
{
	MeshComponent* GateMeshComponent = nullptr;
	CollisionComponent* GateCollisionComponent = nullptr;
	int TextureIndex = 0;
public:
	OrbGate() : WorldObject(ObjectDescription("Orb Gate", 10)) {}

	void Begin() override;
	void OnPropertySet() override;
};