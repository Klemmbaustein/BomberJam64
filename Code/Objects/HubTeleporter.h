#pragma once
#include <Objects/WorldObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>
#include <Objects/Components/ParticleComponent.h>

class HubTeleporter : public WorldObject
{
public:
	HubTeleporter() : WorldObject(ObjectDescription("Hub Teleporter", 9)) {}
	void Begin() override;
	void Tick() override;
	void Destroy() override;
	std::string TargetLevel = "Hub";
	ParticleComponent* TeleportParticles = nullptr;

private:
	MeshComponent* TeleporterMesh = nullptr;
	CollisionComponent* TeleportCollison = nullptr;
};