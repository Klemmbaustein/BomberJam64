#include "BombPickup.h"
#include <Rendering/Mesh/JSModel.h>

void BombPickup::Begin()
{
	PickupMesh = new MeshComponent();
	Attach(PickupMesh);
	PickupMesh->Load("BombButton");
	PickupMesh->GetRelativeTransform().Location += Vector3(0, 0, 0);

	auto CollMesh = JSM::LoadJSModel(Assets::GetAsset("Cube.jsm"));

	PickupCollision = new CollisionComponent();
	Attach(PickupCollision);
	PickupCollision->Init(CollMesh.Vertices[0], CollMesh.Indices[0], Transform(Vector3(0, 2, 0), Vector3(), Vector3(2)));
}

void BombPickup::Tick()
{
}

void BombPickup::Destroy()
{
}
