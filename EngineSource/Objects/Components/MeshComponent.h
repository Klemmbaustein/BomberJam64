#pragma once
#include <Objects/Components/Component.h>
#include <Rendering/Mesh/Model.h>

enum UniformType
{
	U_INT = 0,
	U_FLOAT = 1,
	U_VEC2 = 2,
	U_VEC3 = 3,
	U_TEXTURE = 5
};

class MeshComponent : public Component
{
public:
	virtual void Start() override;
	virtual void Tick() override;
	virtual void Destroy() override;

	void Load(std::string File);
	FrustumCulling::AABB GetBoundingBox();

	void SetUniform(std::string Name, UniformType Type, std::string Content, uint8_t MeshSection);

	Model* GetModel()
	{
		return MeshModel;
	}
	MeshData GetMeshData();
	void SetRelativeTransform(Transform NewRelativeTransform);
	Transform& GetRelativeTransform();

	void SetVisibility(bool NewVisibility);


	bool AutomaticallyUpdateTransform = true;
	void UpdateTransform();
protected:
	Model* MeshModel = nullptr;
	Shader* CurrentShader = nullptr;
	std::string ModelPath;
	Transform RelativeTransform;
};