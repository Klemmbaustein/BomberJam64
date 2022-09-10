#include "MeshComponent.h"
void MeshComponent::Start(WorldObject* Parent)
{
	if (MeshModel)
	{
		MeshModel->ModelTransform = Parent->GetTransform() + RelativeTransform;
	}
	this->Parent = Parent;
}

void MeshComponent::Destroy()
{
	for (int i = 0; i < Graphics::ModelsToRender.size(); i++)
	{
		if (MeshModel == Graphics::ModelsToRender[i])
		{
			Graphics::ModelsToRender.erase(Graphics::ModelsToRender.begin() + i);
		}
	}
	delete MeshModel;
}


void MeshComponent::Load(std::string File)
{
	MeshModel = new Model(Assets::GetAsset(File + ".jsm"));
	ModelPath = File;
	Graphics::ModelsToRender.push_back(MeshModel);
	ModelIndex = Graphics::ModelsToRender.size() - 1;
	MeshModel->UpdateTransform();
}

void MeshComponent::SetActiveShader(Shader* Shader)
{
	CurrentShader = Shader;
	//delete MeshModel;
	//MeshModel = new Model(ModelPath);
}

Collision::Box MeshComponent::GetBoundingBox()
{
	return MeshModel->Size;
}

void MeshComponent::SetUniform(std::string Name, UniformType Type, std::string Content, uint8_t MeshSection)
{
	MeshModel->SetUniform(Uniform(Name, Type, (void*)Content.c_str()), MeshSection);
}

MeshData MeshComponent::GetMeshData()
{
	return MeshModel->ModelMeshData;
}

void MeshComponent::SetRelativeTransform(Transform NewRelativeTransform)
{
	if(Parent)
	RelativeTransform = NewRelativeTransform;
}

Transform& MeshComponent::GetRelativeTransform()
{
	return RelativeTransform;
}

void MeshComponent::SetVisibility(bool NewVisibility)
{
	MeshModel->Visible = NewVisibility;
}
