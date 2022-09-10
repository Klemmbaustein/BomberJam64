#include "WallObject.h"

void WallObject::Begin()
{
	WallMesh = new MeshComponent();
	Attach(WallMesh);
	WallMesh->Load("Cube");
	WallMesh->SetRelativeTransform(Transform(Vector3(), Vector3(), Vector3(2)));

	WallCollision = new CollisionComponent();
	Attach(WallCollision);
	WallCollision->Init(WallMesh->GetMeshData().Vertices, WallMesh->GetMeshData().Indices, Transform(Vector3(), Vector3(), Vector3(2)));

}

void WallObject::Tick()
{
}

void WallObject::Destroy()
{
}
