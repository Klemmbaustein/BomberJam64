#pragma once
#include "Objects/WorldObject.h"

namespace Objects
{
	//Creating or destroying objects is expensive and should be avoided if possible
	template<typename T>
	T* SpawnObject(Transform ObjectTransform);

	bool DestroyObject(WorldObject* Object);

	WorldObject* SpawnObjectFromID(uint32_t ID, Transform ObjectTransform);
	extern const std::vector<ObjectDescription> EditorObjects;
	extern std::vector<WorldObject*> AllObjects;
	void CreateNewMeshObject(std::string FilePath, Transform Transform, std::string Name = "NONE");
	std::vector<WorldObject*> GetAllObjectsWithID(uint32_t ID);
}