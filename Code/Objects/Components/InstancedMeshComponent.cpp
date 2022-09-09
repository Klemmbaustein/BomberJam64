#include "InstancedMeshComponent.h"
#include <WorldParameters.h>

InstancedMeshComponent::InstancedMeshComponent(std::string File)
{
	Mesh = new InstancedModel(Assets::GetAsset(File));
	Graphics::ModelsToRender.push_back(Mesh);
}

void InstancedMeshComponent::Start(WorldObject* Parent)
{
	this->Parent = Parent;
}

void InstancedMeshComponent::Tick()
{
}

void InstancedMeshComponent::Destroy()
{
	for (int i = 0; i < Graphics::ModelsToRender.size(); i++)
	{
		if (Mesh == Graphics::ModelsToRender[i])
		{
			Graphics::ModelsToRender.erase(Graphics::ModelsToRender.begin() + i);
		}
	}
	delete Mesh;
}

size_t InstancedMeshComponent::AddInstance(Transform T)
{
	return Mesh->AddInstance(T + Parent->GetTransform());
}

void InstancedMeshComponent::UpdateInstances()
{
	Mesh->ConfigureVAO();
}
