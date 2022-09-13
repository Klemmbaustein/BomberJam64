#include "CollisionComponent.h"
#include <WorldParameters.h>
#include <Math/Collision/Collision.h>


Collision::HitResponse CollisionComponent::OverlapCheck(std::set<CollisionComponent*> MeshesToIgnore)
{
	CollMesh.SetTransform((LastParentTransform + RelativeTransform));
	return CollMesh.OverlapCheck(MeshesToIgnore);
}

void CollisionComponent::Destroy()
{
	for (int i = 0; i < Collision::CollisionBoxes.size(); i++)
	{
		if (Collision::CollisionBoxes.at(i) == this)
		{
			Collision::CollisionBoxes.erase(Collision::CollisionBoxes.begin() + i);
		}
	}
}

void CollisionComponent::Tick()
{
	if (LastParentTransform != GetParent()->GetTransform())
	{
		LastParentTransform = GetParent()->GetTransform();
		CollMesh.SetTransform((LastParentTransform + RelativeTransform));
	}
}

void CollisionComponent::Init(std::vector<Vertex> Vertices, std::vector<int> Indices, Transform RelativeTransform)
{
	this->RelativeTransform = RelativeTransform;
	this->RelativeTransform.Scale = this->RelativeTransform.Scale * 0.025f;
	CollMesh = Collision::CollisionMesh(Vertices, Indices, GetParent()->GetTransform() + this->RelativeTransform);
	Collision::CollisionBoxes.push_back(this);
}