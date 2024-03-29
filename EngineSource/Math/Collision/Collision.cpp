#include "Collision.h"
#include <algorithm>
#include <Math/Collision/TriangleIntersect.h>
#include <Objects/Components/CollisionComponent.h>
#include <Engine/Log.h>
#include <cmath>

bool Collision::CollisionAABB(Box a, Box b)
{
	return (a.minX <= b.maxX && a.maxX >= b.minX) &&
		(a.minY <= b.maxY && a.maxY >= b.minY) &&
		(a.minZ <= b.maxZ && a.maxZ >= b.minZ);
}


Collision::HitResponse Collision::BoxOverlapCheck(Collision::Box a, Vector3 Offset)
{
	//Offset = Offset;
	//a = a + Offset;
	//for (int i = 0; i < Collision::CollisionBoxes.size(); i++)
	//{
	//	bool NewCollision = CollisionAABB(a, Collision::CollisionBoxes.at(i)->Box.TransformBy(Collision::CollisionBoxes[i]->GetParent()->GetTransform()
	//		+ Collision::CollisionBoxes[i]->RelativeTransform));
	//	if (NewCollision)
	//	{
	//		Collision::HitResponse NewHitResponse(true, Vector3(), Vector3(), 0, Collision::CollisionBoxes[i]->GetParent());
	//		NewHitResponse.HitComponent = Collision::CollisionBoxes[i];
	//		return NewHitResponse;
	//	}
	//}
	throw "Depricated function: Collision::BoxOverlapCheck(Collision::Box a, Vector3 Offset)";
	return Collision::HitResponse(false, Vector3(), 0);
}


bool Collision::IsPointIn3DBox(Box a, Vector3 p)
{
	return (a.minX <= p.X && a.maxX >= p.X) &&
		(a.minY <= p.Y && a.maxY >= p.Y) &&
		(a.minZ <= p.Z && a.maxZ >= p.Z);
}
//https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code
// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting, 
// returns t value of intersection and intersection point q 
Collision::HitResponse IntersectRaySphere(Vector3 Start, Vector3 Dir, Vector3 sphere, float radius)
{
	return Collision::HitResponse(true, 0, Vector3(), 0, nullptr);
	Vector3 m = Start - sphere;
	float b = Vector3::Dot(m, Dir);
	float c = Vector3::Dot(m, m) - radius * radius;
	// Exit if r�s origin outside s (c > 0) and r pointing away from s (b > 0) 
	if (c > 0.0f && b > 0.0f) return Collision::HitResponse();
	float discr = b * b - c;

	// A negative discriminant corresponds to ray missing sphere 
	//if (discr < 0.0f) return Collision::HitResponse();

	// Ray now found to intersect sphere, compute smallest t value of intersection
	float t = -b - sqrt(discr);

	// If t is negative, ray started inside sphere so clamp t to zero 
	if (t < 0.0f) t = 0.0f;
	Vector3 q = Start + t * Dir;

	return Collision::HitResponse(true, q, Vector3(), t, nullptr);
}


bool SpheresOverlapping(glm::vec3 Sphere1Pos, float Sphere1Radius, glm::vec3 Sphere2Pos, float Sphere2Radius)
{
	return glm::distance(Sphere1Pos, Sphere2Pos) < Sphere1Radius + Sphere2Radius;
}


Collision::HitResponse Collision::LineTrace(Vector3 RayStart, Vector3 RayEnd, std::set<CollisionComponent*> MeshesToIgnore)
{
	float CollTMin = INFINITY;
	HitResponse Collision;

	for (int i = 0; i < Collision::CollisionBoxes.size(); i++)
	{
		if (MeshesToIgnore.find(Collision::CollisionBoxes[i]) == MeshesToIgnore.end())
		{
			HitResponse NewCollision = Collision::CollisionBoxes.at(i)->CollMesh.CheckAgainstLine(RayStart, RayEnd);
			if (NewCollision.t < CollTMin && NewCollision.Hit)
			{
				CollTMin = NewCollision.t;
				Collision = NewCollision;
				Collision.Normal = NewCollision.Normal;
				Collision.HitComponent = Collision::CollisionBoxes.at(i);
				Collision.HitObject = Collision::CollisionBoxes.at(i)->GetParent();
			}
		}
	}
	return Collision;
}

//send help
Collision::HitResponse Collision::LineCheckForAABB(Collision::Box a, Vector3 RayStart, Vector3 RayEnd)
{
	Vector3 Normal;
	Vector3 RayDir = RayEnd - RayStart;
	float tmp;

	float txMin = (a.minX - RayStart.X) / RayDir.X;
	float txMax = (a.maxX - RayStart.X) / RayDir.X;
	if (txMax < txMin)
	{
		tmp = txMax;
		txMax = txMin;
		txMin = tmp;
	}
	float tyMin = (a.minY - RayStart.Y) / RayDir.Y;
	float tyMax = (a.maxY - RayStart.Y) / RayDir.Y;
	if (tyMax < tyMin)
	{
		tmp = tyMax;
		tyMax = tyMin;
		tyMin = tmp;
	}
	float tzMin = (a.minZ - RayStart.Z) / RayDir.Z;
	float tzMax = (a.maxZ - RayStart.Z) / RayDir.Z;
	if (tzMax < tzMin)
	{
		tmp = tzMax;
		tzMax = tzMin;
		tzMin = tmp;
	}
	float tMin;
	if (txMin > tyMin)
	{
		tMin = txMin;
		Normal = Vector3(-1, 0, 0);
	}
	else
	{
		tMin = tyMin;
		Normal = Vector3(0, -1, 0);
	}
	float tMax = (txMax < tyMax) ? txMax : tyMax;
	if (txMin > tyMax || tyMin > txMax) return HitResponse();
	if (tMin > tzMax || tzMin > tMax) return HitResponse();
	if (tzMin > tMin)
	{
		tMin = tzMin;
		Normal = Vector3(0, 0, -1);
	}
	if (tMax < tzMax) tMax = tzMax;
	Vector3 ImpactLocation = (RayDir * tMin);
	Vector3 RayDirNormalized = Vector3(RayDir.X > 0 ? 1 : -1, RayDir.Y > 0 ? 1 : -1, RayDir.Z > 0 ? 1 : -1);
	Normal = Normal * RayDirNormalized;
	if (tMin < 0)
	{
		if (!IsPointIn3DBox(a, RayStart))
		{
			return HitResponse();
		}
	}
	return HitResponse(true, ImpactLocation + RayStart, Normal, tMin);
}



Collision::HitResponse testRayThruTriangle(glm::vec3 orig, glm::vec3 end, glm::vec3 A, glm::vec3 B, glm::vec3 C)
{
	glm::vec3 dir = end;
	glm::vec3 E1 = C - A;
	glm::vec3 E2 = B - A;
	glm::vec3 N = glm::cross(E1, E2);
	float det = -glm::dot(dir, N);
	float invdet = 1.0 / det;
	glm::vec3 AO = orig - A;
	glm::vec3 DAO = glm::cross(AO, dir);
	float u = dot(E2, DAO) * invdet;
	float v = -dot(E1, DAO) * invdet;
	float t = dot(AO, N) * invdet;
	if ((det >= 1e-6 && t >= 0.0 && u >= 0.0 && v >= 0.0 && (u + v) <= 1.0))
		return Collision::HitResponse(true, orig + end * t, normalize(N), t);
	else return Collision::HitResponse();
}



Collision::CollisionMesh::CollisionMesh(std::vector<Vertex> Verts, std::vector<int> Indices, Transform T)
{
	//T.Scale = T.Scale * 2.5f;
	RawVertices = Verts;
	this->Indices = Indices;
	ModelMatrix = T.ToMatrix();

	WorldPosition = T.Location;
	WorldScale = std::max(std::max(T.Scale.X, T.Scale.Y), T.Scale.Z);
	ApplyMatrix();
}

Collision::CollisionMesh::~CollisionMesh()
{
}

glm::mat4 Collision::CollisionMesh::GetMatrix()
{
	return ModelMatrix;
}

void Collision::CollisionMesh::SetTransform(Transform T)
{
	//T.Scale = T.Scale * 2.5f;
	glm::mat4 newM = T.ToMatrix();
	if (newM != ModelMatrix)
	{
		ModelMatrix = newM;
		WorldPosition = T.Location;
		WorldScale = std::max(std::max(T.Scale.X, T.Scale.Y), T.Scale.Z);
		ApplyMatrix();
	}
}

void Collision::CollisionMesh::ApplyMatrix()
{
	Vertices.resize(RawVertices.size());
	for (unsigned int i = 0; i < RawVertices.size(); i++)
	{
		Vertices[i] = RawVertices[i];
		float dist = glm::distance(Vertices[i].Position, glm::vec3(0));
		if (dist > SphereCollisionSize)
		{
			SphereCollisionSize = dist;
		}
		Vertices[i].Position = ModelMatrix * glm::vec4(Vertices[i].Position, 1);
		Vertices[i].Normal = glm::normalize(glm::mat3(ModelMatrix) * Vertices[i].Normal);
	}
	SphereCollisionSize *= WorldScale;
}

//Wow, this function sucks
Collision::HitResponse Collision::CollisionMesh::CheckAgainstMesh(CollisionMesh* b)
{
	if (SpheresOverlapping(WorldPosition, SphereCollisionSize, b->WorldPosition, b->SphereCollisionSize))
	{
		Collision::HitResponse r;
		r.Hit = false;
		for (unsigned int i = 0; i < Indices.size(); i += 3)
		{
			for (int32_t j = 0; j < b->Indices.size(); j += 3)
			{
				float a1[3], b1[3], c1[3], a2[3], b2[3], c2[3];
				//Tri 1
				a1[0] = Vertices[Indices[i]].Position.x; a1[1] = Vertices[Indices[i]].Position.y; a1[2] = Vertices[Indices[i]].Position.z;
				b1[0] = Vertices[Indices[i + 1]].Position.x; b1[1] = Vertices[Indices[i + 1]].Position.y; b1[2] = Vertices[Indices[i + 1]].Position.z;
				c1[0] = Vertices[Indices[i + 2]].Position.x; c1[1] = Vertices[Indices[i + 2]].Position.y; c1[2] = Vertices[Indices[i + 2]].Position.z;
				//Tri 2
				a2[0] = b->Vertices[b->Indices[j]].Position.x; a2[1] = b->Vertices[b->Indices[j]].Position.y; a2[2] = b->Vertices[b->Indices[j]].Position.z;
				b2[0] = b->Vertices[b->Indices[j + 1]].Position.x; b2[1] = b->Vertices[b->Indices[j + 1]].Position.y; b2[2] = b->Vertices[b->Indices[j + 1]].Position.z;
				c2[0] = b->Vertices[b->Indices[j + 2]].Position.x; c2[1] = b->Vertices[b->Indices[j + 2]].Position.y; c2[2] = b->Vertices[b->Indices[j + 2]].Position.z;
				if (tri_tri_overlap_test_3d(a1, b1, c1, a2, b2, c2))
				{
					r.Hit = true;
					Vector3 n = Vector3::Cross(
						Vector3(b2[0], b2[1], b2[2]) - Vector3(a2[0], a2[1], a2[2]),
						Vector3(c2[0], c2[1], c2[2]) - Vector3(a2[0], a2[1], a2[2]));
					r.Normal = n;
					if (std::to_string(n.X) == "-nan(ind)")
					{
						//the triangle has a surface area of 0, we skip it (if thats the case, all elements of the normal vector are -nan(ind))
						continue;
					}
					return r;
				}

			}
		}
	}
	return Collision::HitResponse();
}

Collision::HitResponse Collision::CollisionMesh::CheckAgainstLine(Vector3 RayStart, Vector3 RayEnd)
{
	Collision::Box BroadPhaseBox = Collision::Box(
		SpherePosition.X - SphereCollisionSize, SpherePosition.X + SphereCollisionSize,
		SpherePosition.Y - SphereCollisionSize, SpherePosition.Y + SphereCollisionSize,
		SpherePosition.Z - SphereCollisionSize, SpherePosition.Z + SphereCollisionSize).TransformBy(Transform(WorldPosition, Vector3(), Vector3(1)));
	if (LineCheckForAABB(BroadPhaseBox,
		RayStart, RayEnd).Hit)
	{
		Vector3 Dir = RayEnd - RayStart;
		Collision::HitResponse r;
		r.Hit = false;
		r.t = INFINITY;

		for (size_t i = 0; i < Indices.size(); i += 3)
		{
			Vector3 CurrentTriangle[3] = { Vertices[Indices[i]].Position, Vertices[Indices[i + 2]].Position, Vertices[Indices[i + 1]].Position };
			Collision::HitResponse newR
				= (testRayThruTriangle(RayStart, Dir, CurrentTriangle[0], CurrentTriangle[1], CurrentTriangle[2]));
			if (newR.t < r.t && newR.Hit && newR.t < 1)
			{
				r = newR;
			}
		}
		return r;
	}
	else
	{
	}
	return Collision::HitResponse();
}

Collision::HitResponse Collision::CollisionMesh::CheckAgainstAABB(const Box& b)
{
	return Collision::HitResponse();
}
Collision::HitResponse Collision::CollisionMesh::OverlapCheck(std::set<CollisionComponent*> MeshesToIgnore)
{
	for (CollisionComponent* c : Collision::CollisionBoxes)
	{
		if (MeshesToIgnore.find(c) == MeshesToIgnore.end())
		{
			if (&c->CollMesh != this)
			{
				Collision::HitResponse newR = this->CheckAgainstMesh(&c->CollMesh);
				if (newR.Hit)
				{
					newR.HitObject = c->GetParent();
					newR.HitComponent = c;
					return newR;
				}
			}
		}
	}
	return Collision::HitResponse();
}
void Collision::RunTest()
{
	float a1[3] = { 0, -1, -1 };
	float a3[3] = { 0, 1, 0 };
	float a2[3] = { 0, -1, 1 };
	float b1[3] = { -1, 0, -1 };
	float b2[3] = { 1, 0, 0 };
	float b3[3] = { -1, 0, 1 };
	Log::CreateNewLogMessage(tri_tri_overlap_test_3d(a1, a2,
		a3, b1, b2, b3) ? "TRUE" : "FALSE");
	//test();
}
