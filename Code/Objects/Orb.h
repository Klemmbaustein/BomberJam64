#pragma once
#include <Objects/WorldObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>
#include <Objects/Components/ParticleComponent.h>
#include <GENERATED/GENERATED_Orb.h>

class Orb : public WorldObject
{
public:
	ORB_GENERATED()
	void Begin() override;
	void Tick() override;
	void Destroy() override;

private:
	ParticleComponent* GlowParticle = nullptr;
	MeshComponent* OrbMesh = nullptr;
	CollisionComponent* OrbCollision = nullptr;
};