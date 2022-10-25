#include "Orb.h"
#include <World/Stats.h>

void Orb::Begin()
{
	OrbMesh = new MeshComponent();
	Attach(OrbMesh);
	OrbMesh->Load("Orb");
	OrbMesh->GetRelativeTransform().Location = Vector3(0.5);

	OrbCollision = new CollisionComponent();
	Attach(OrbCollision);
	OrbCollision->Init(OrbMesh->GetMeshData().Vertices, OrbMesh->GetMeshData().Indices);

	GlowParticle = new ParticleComponent();
	Attach(GlowParticle);
	GlowParticle->LoadParticle("Orb");

	LightComponent = new PointLightComponent();
	Attach(LightComponent);
	LightComponent->SetFalloff(2);
	LightComponent->SetIntensity(250);
	LightComponent->SetColor(Vector3(0, 0.5, 1));
	LightComponent->SetRelativeLocation(0);
}

void Orb::Destroy()
{
}

void Orb::Tick()
{
	OrbMesh->GetRelativeTransform().Rotation.Y += Performance::DeltaTime;
}