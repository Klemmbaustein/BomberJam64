#pragma once
#include <Objects/Components/InstancedMeshComponent.h>
#include <Objects/Objects.h>

class InstancedMeshObject : public WorldObject
{
public:
	virtual void Destroy() override;
	virtual void Tick() override;
	virtual void Begin() override;
	void LoadFromFile(std::string Filename);
	virtual void OnPropertySet() override;
	size_t AddInstance(Transform T);
	InstancedMeshObject() : WorldObject(ObjectDescription("Instanced Mesh", 3)) {}


	virtual std::string Serialize() override;
	virtual void Deserialize(std::string SerializedObject) override;
protected:
	bool Initialized = true, SoonInitialized = true;
	int Ammount = 50;
	int Range = 1000;
	Vector3 Scale = Vector3(1);
	std::string Filename;
	std::string ComponentName;
	InstancedMeshComponent* IMComponent = nullptr;
};