#pragma once
#include "Math/Vector.h"
#include "Rendering/Mesh/Model.h"
#include "CollisionBox.h"
#include "Objects/WorldObject.h"
#include <set>


class CollisionComponent;

namespace Collision
{

	struct HitResponse
	{
		bool Hit;
		Vector3 ImpactPoint;
		float t = INFINITY;
		WorldObject* HitObject = nullptr;
		CollisionComponent* HitComponent = nullptr;
		Vector3 Normal;
		HitResponse()
		{
			Hit = false;
			ImpactPoint = Vector3();
		}
		HitResponse(bool Hit, Vector3 ImpactPoint, Vector3 Normal, float t = INFINITY, WorldObject* InModel = nullptr)
		{
			this->Hit = Hit;
			this->ImpactPoint = ImpactPoint;
			this->t = t;
			this->Normal = Normal;
			HitObject = InModel;
		}
	};
	struct CollisionMesh
	{
		CollisionMesh(std::vector<Vertex> Verts, std::vector<int> Indices, Transform T);
		CollisionMesh() { ModelMatrix = glm::mat4(1); }
		~CollisionMesh();
		glm::mat4 GetMatrix();
		void SetTransform(Transform T);
		Collision::HitResponse CheckAgainstMesh(CollisionMesh* b);
		Collision::HitResponse CheckAgainstLine(Vector3 RayStart, Vector3 RayEnd);
		Collision::HitResponse CheckAgainstAABB(const Box& b);
		float SphereCollisionSize = 0;
		Vector3 SpherePosition;
		HitResponse OverlapCheck(std::set<CollisionComponent*> MeshesToIgnore = {});
	protected:
		Vector3 WorldPosition;
		float WorldScale = 1;
		void ApplyMatrix();
		glm::mat4 ModelMatrix;
		std::vector<Vertex> Vertices, RawVertices; std::vector<int> Indices;

	};
	bool CollisionAABB(Box a, Box b);


	HitResponse BoxOverlapCheck(Box a, Vector3 Offset);
	void RunTest();
	bool IsPointIn3DBox(Box a, Vector3 p);
	HitResponse LineTrace(Vector3 RayStart, Vector3 RayEnd, std::set<CollisionComponent*> MeshesToIgnore = {});

	HitResponse LineCheckForAABB(Box b, Vector3 RayStart, Vector3 RayEnd);
}