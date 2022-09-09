#pragma once
#include "WorldObject.h"

namespace Objects
{
	//Creating or destroying objects is expensive and should be avoided if possible
	template<typename T>
	T* SpawnObject(Transform ObjectTransform);

	bool DestroyObject(WorldObject* Object);

	WorldObject* SpawnObjectFromID(uint32_t ID, Transform ObjectTransform);
	extern const std::vector<ObjectDescription> EditorObjects;
}