#include "Orb.h"

void Orb::Begin()
{
	OrbMesh = new MeshComponent();
	Attach(OrbMesh);
	OrbMesh->Load("Orb");
	OrbMesh->GetRelativeTransform().Location = Vector3(0.5);

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