#pragma once
#include <Objects/Objects.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>
#include <GENERATED/GENERATED_OrbGate.h>

class OrbGate : public WorldObject
{
	MeshComponent* GateMeshComponent = nullptr;
	CollisionComponent* GateCollisionComponent = nullptr;
	int TextureIndex = 0;
public:
	ORBGATE_GENERATED()

	void Begin() override;
	void OnPropertySet() override;
};