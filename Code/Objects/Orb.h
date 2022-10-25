#pragma once
#include <Objects/WorldObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>
#include <Objects/Components/ParticleComponent.h>
#include <Objects/Components/PointLightComponent.h>

class Orb : public WorldObject
{
public:
	Orb() : WorldObject(ObjectDescription("Orb", 8)) {}
	void Begin() override;
	void Tick() override;
	void Destroy() override;

private:
	ParticleComponent* GlowParticle = nullptr;
	MeshComponent* OrbMesh = nullptr;
	CollisionComponent* OrbCollision = nullptr;
	PointLightComponent* LightComponent = nullptr;
};