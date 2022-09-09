#pragma once
#include "Objects/WorldObject.h"
#include "WorldParameters.h"
#include <fstream>
#include <Objects/MeshObject.h>


namespace World
{
	extern Camera* DefaultCamera;

	struct Object
	{
		Transform ObjectTransform;
	};

	extern std::string CurrentScene;

	void LoadNewScene(std::string FilePath);
	void SaveSceneAs(std::string FilePath, bool Subscene = false);
	void LoadSubScene(std::string FilePath);

	void Tick();
	void ScheduleLoadNewScene(std::string FilePath);
}