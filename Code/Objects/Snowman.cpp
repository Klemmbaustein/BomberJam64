#include "Snowman.h"
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>
#include <World/Stats.h>
void Snowman::Begin()
{
	MeshComponent* NewMeshComponent = new MeshComponent();
	Attach(NewMeshComponent);
	NewMeshComponent->Load("Snowman");
	NewMeshComponent->SetRelativeTransform(Transform(Vector3(0, 1, 0), Vector3(), Vector3(1)));

	if (IsInEditor)
	{
		CollisionComponent* NewCollisionComponent = new CollisionComponent();
		Attach(NewCollisionComponent);
		NewCollisionComponent->Init(NewMeshComponent->GetMeshData().Vertices, NewMeshComponent->GetMeshData().Indices);
	}
}
