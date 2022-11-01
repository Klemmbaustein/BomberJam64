#include "Objects.h"
#include <Engine/Scene.h>
#include <Objects/MeshObject.h>
#include <Objects/SoundObject.h>
#include <Objects/InstancedMeshObject.h>
#include <Objects/PlayerObject.h>
#include <Objects/Bomb.h>
#include <Objects/WallObject.h>
#include <Objects/BombPickup.h>
#include <Objects/Orb.h>
#include <Objects/HubTeleporter.h>
#include <Objects/OrbGate.h>
#include <Objects/Snowman.h>
#include <Objects/Backpack.h>
#include <Engine/Log.h>

template<typename T>
inline T* Objects::SpawnObject(Transform ObjectTransform)
{
	T* NewObject = new T();
	NewObject->Start(NewObject->GetObjectDescription().Name, ObjectTransform);
	NewObject->CurrentScene = World::CurrentScene;
	return NewObject;
}
bool Objects::DestroyObject(WorldObject* Object)
{
	if (Object)
	{
		Object->Destroy();
		for (Component* LoopComponent : Object->GetComponents())
		{
			LoopComponent->Destroy();
			delete LoopComponent;
		}
		for (int i = 0; i < Objects::AllObjects.size(); i++)
		{
			if (Object)
			{
				if (Objects::AllObjects.at(i) == Object)
				{
					Objects::AllObjects.erase(Objects::AllObjects.begin() + i);
				}
			}
		}
		delete Object;
		return true;
	}
	return false;
}
WorldObject* Objects::SpawnObjectFromID(uint32_t ID, Transform ObjectTransform)
{
	switch (ID)
	{
#include <GENERATED/GENERATED_Spawnlist.h>
	default:
		Log::CreateNewLogMessage("Tried to spawn " + std::to_string(ID) + " but that ID does not exist!");
		throw "Attempted to spawn unknown object";
	}
}


namespace Objects
{

	const std::vector<ObjectDescription> EditorObjects
	{
#include <GENERATED/GENERATED_ListOfObjects.h>
	};
}