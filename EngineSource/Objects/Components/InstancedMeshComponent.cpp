#include "InstancedMeshComponent.h"
#include <World/Graphics.h>
#include <World/Assets.h>
#include <Rendering/Utility/Framebuffer.h>
InstancedMeshComponent::InstancedMeshComponent(std::string File)
{
	Mesh = new InstancedModel(Assets::GetAsset(File));
	Graphics::MainFramebuffer->Renderables.push_back(Mesh);
}

void InstancedMeshComponent::Start()
{
}

void InstancedMeshComponent::Tick()
{
}

void InstancedMeshComponent::Destroy()
{
	for (auto* f : Graphics::AllFramebuffers)
	{
		for (int i = 0; i < f->Renderables.size(); i++)
		{
			if (Mesh == f->Renderables[i])
			{
				f->Renderables.erase(f->Renderables.begin() + i);
			}
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
