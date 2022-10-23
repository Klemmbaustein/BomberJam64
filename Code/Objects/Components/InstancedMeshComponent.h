#pragma once
#include <Objects/Components/Component.h>
#include <Rendering/Mesh/InstancedModel.h>

class InstancedMeshComponent : public Component
{
public:
	InstancedMeshComponent(std::string File);
	void Start() override;
	void Tick() override;
	void Destroy() override;
	size_t AddInstance(Transform T);
	void UpdateInstances();
protected:
	InstancedModel* Mesh;
};