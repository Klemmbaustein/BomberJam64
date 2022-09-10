#include "InstancedMeshObject.h"
#include <EngineRandom.h>
#include <Math/Collision/Collision.h>
#include <WorldParameters.h>
void InstancedMeshObject::Destroy()
{
}

void InstancedMeshObject::Tick()
{
	if (!Initialized)
	{
		for (int i = 0; i < Ammount; i++)
		{
			Vector3 Loc = Vector3(Random::GetRandomNumber(-Range, Range),
				0, Random::GetRandomNumber(-Range, Range));

			Collision::HitResponse Hit = Collision::LineTrace(ObjectTransform.Location + Loc, ObjectTransform.Location + Loc - Vector3(0, 100, 0));
			Vector3 AxisA = Vector3(Hit.Normal.Y, Hit.Normal.Z, Hit.Normal.X);
			Vector3 AxisB = Vector3::Cross(Hit.Normal, AxisA);
			if (Hit.Hit)
			{
				if (ComponentName == "")
				{
					AddInstance(Transform(Hit.ImpactPoint - ObjectTransform.Location,
						Vector3::LookAtFunction(Vector3(), AxisB),
						Scale * Random::GetRandomNumber(15, 25) / 20));
				}
				else if (ComponentName == Hit.HitObject->GetName())
				{
					AddInstance(Transform(Hit.ImpactPoint - ObjectTransform.Location,
						Vector3::LookAtFunction(Vector3(), AxisB),
						Scale * Random::GetRandomNumber(15, 25) / 20));
				}
			}
			else
			{
			}
		}
		IMComponent->UpdateInstances();
		Initialized = true;
	}
	if (!SoonInitialized)
	{
		Initialized = false;
		SoonInitialized = true;
	}
}

void InstancedMeshObject::Begin()
{
	Properties.push_back(Object::Property("Mesh", T_STRING, &Filename));
	Properties.push_back(Object::Property("NumInstances", T_INT, &Ammount));
	Properties.push_back(Object::Property("Range", T_INT, &Range));
	Properties.push_back(Object::Property("Component Name", T_STRING, &ComponentName));
	Properties.push_back(Object::Property("Scale", T_VECTOR3, &Scale));

	IMComponent = new InstancedMeshComponent("");
	Attach(IMComponent);
	IMComponent->UpdateInstances();

}

void InstancedMeshObject::LoadFromFile(std::string Filename)
{
}

void InstancedMeshObject::OnPropertySet()
{
	Detach(IMComponent);
	IMComponent = new InstancedMeshComponent(Filename + ".jsm");
	Attach(IMComponent);
	SoonInitialized = false;
}

size_t InstancedMeshObject::AddInstance(Transform T)
{
	return IMComponent->AddInstance(T);
}

std::string InstancedMeshObject::Serialize()
{
	return std::string();
}

void InstancedMeshObject::Deserialize(std::string SerializedObject)
{
}
