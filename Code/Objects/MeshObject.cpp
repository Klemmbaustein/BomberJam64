#include "MeshObject.h"
#include "WorldParameters.h"


void MeshObject::Destroy()
{
}

void MeshObject::Tick()
{
	
}

void MeshObject::Begin()
{
	Properties.push_back(Object::Property("Collision Scale", T_VECTOR3, &CollisionScale));
}

void MeshObject::LoadFromFile(std::string Filename)
{
	Mesh = new MeshComponent();
	Mesh->Load(Filename);
	Attach(Mesh);
	this->Filename = Filename;
	if (Mesh->GetModel()->HasCollision)
	{
		MeshCollision = new CollisionComponent();
		Attach(MeshCollision);
		MeshCollision->Init(Mesh->GetMeshData().Vertices, Mesh->GetMeshData().Indices, Transform(Vector3(), Vector3(), Vector3(1)));
	}
}

void MeshObject::OnPropertySet()
{
	if(Mesh->GetModel()->HasCollision)
	MeshCollision->CollMesh.SetTransform(Transform(Vector3(), Vector3(), CollisionScale));
}

std::string MeshObject::Serialize()
{
	return Filename;
}

void MeshObject::Deserialize(std::string SerializedObject)
{
	LoadFromFile(SerializedObject);
}