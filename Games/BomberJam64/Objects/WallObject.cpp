#include "WallObject.h"
#include <Objects/Objects.h>
#include <Objects/Orb.h>

void WallObject::Begin()
{
	WallMesh = new MeshComponent();
	Attach(WallMesh);
	WallMesh->Load(MeshName);
	WallMesh->SetRelativeTransform(Transform(Vector3(), Vector3(), Vector3(2)));

	WallCollision = new CollisionComponent();
	Attach(WallCollision);
	WallCollision->Init(WallMesh->GetMeshData().Vertices, WallMesh->GetMeshData().Indices, Transform(Vector3(), Vector3(), Vector3(2)));

	Properties.push_back(Object::Property("Mesh", T_STRING, (void*)&MeshName));
	Properties.push_back(Object::Property("Has Orb", T_INT, (void*)&ContainsOrb));

}

void WallObject::Tick()
{
}

void WallObject::Destroy()
{
	if (ContainsOrb)
	{
		Objects::SpawnObject<Orb>(GetTransform());
	}
}

void WallObject::OnPropertySet()
{
	Detach(WallMesh);
	Detach(WallCollision);
	WallMesh = new MeshComponent();
	Attach(WallMesh);
	WallMesh->Load(MeshName);
	WallMesh->SetRelativeTransform(Transform(Vector3(), Vector3(), Vector3(2)));

	WallCollision = new CollisionComponent();
	Attach(WallCollision);
	WallCollision->Init(WallMesh->GetMeshData().Vertices, WallMesh->GetMeshData().Indices, Transform(Vector3(), Vector3(), Vector3(2)));
}
