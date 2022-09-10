#include "Orb.h"

void Orb::Begin()
{
	OrbMesh = new MeshComponent();
	Attach(OrbMesh);
	OrbMesh->Load("Orb");

	OrbCollision = new CollisionComponent();
	Attach(OrbCollision);
	OrbCollision->Init(OrbMesh->GetMeshData().Vertices, OrbMesh->GetMeshData().Indices);
}

void Orb::Destroy()
{
}

void Orb::Tick()
{
	OrbMesh->GetRelativeTransform().Rotation.Y += Performance::DeltaTime;
}