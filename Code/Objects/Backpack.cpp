#include "Backpack.h"
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>

void Backpack::Begin()
{
	MeshComponent* m = new MeshComponent();
	Attach(m);
	m->Load("Backpack");

	CollisionComponent* c = new CollisionComponent();
	Attach(c);
	c->Init(m->GetMeshData().Vertices, m->GetMeshData().Indices);
}

void Backpack::Tick()
{
	GetTransform().Rotation.Y += Performance::DeltaTime;
}
