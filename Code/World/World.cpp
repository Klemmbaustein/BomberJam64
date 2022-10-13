#include <World/Assets.h>
#include <World/Graphics.h>
#include <World/Stats.h>

#include <fstream>
#include "Objects/MeshObject.h"
#include <iostream>
#include <filesystem>
#include <Objects/Components/CollisionComponent.h>
#include "Engine.h"
#include <UI/Default/ScrollObject.h>
#include <FileUtility.h>
#include <Scene.h>
#include <Rendering/Utility/SSAO.h>
#include <Log.h>

#include <SDL.h>

namespace Engine
{
	std::string CurrentProjectName = ProjectName;
	std::string VersionString = VERSION_STRING + std::string(IS_IN_EDITOR ? "-Editor" : "-Build");
}
namespace Graphics
{
	std::vector<Light> Lights;
	Sun WorldSun;
	Fog WorldFog;
	unsigned int ShadowResolution = 2000;
	std::vector<Renderable*> ModelsToRender;
	std::vector<UICanvas*> UIToRender;
	Vector2 WindowResolution(1600, 900);
	unsigned int PCFQuality = 0;
	void SetWindowResolution(Vector2 NewResolution)
	{
		WindowResolution = NewResolution;
		SSAO::ResizeBuffer(NewResolution.X, NewResolution.Y);
	}
	void RecompileShaders()
	{
		Uint64 PerfCounterFrequency = SDL_GetPerformanceFrequency();
		Uint64 LastCounter = SDL_GetPerformanceCounter();
		UIShader->Recompile();
		TextShader->Recompile();
		MainShader->Recompile();
		Uint64 EndCounter = SDL_GetPerformanceCounter();
		Uint64 counterElapsed = EndCounter - LastCounter;
		float LoadTime = ((float)counterElapsed) / ((float)PerfCounterFrequency);
		Log::CreateNewLogMessage(std::string("Recompiled Shaders. (").append(std::to_string(LoadTime)).append(" seconds)"), Vector3(1, 0.6f, 0.f));
	}
	bool Bloom = true, FXAA = false;
	float Gamma = 1;
	float ChrAbbSize = 0, Vignette = 0.2;
	Camera* MainCamera;
	Shader* MainShader;
	Shader* ShadowShader;
	Shader* TextShader;
	Shader* UIShader;
	Vector3 LightRotation = Vector3(0.5f, 0.8f, 0.7f);
	std::vector<CollisionComponent*> Objects;
	bool IsRenderingShadows = false;
	namespace UI
	{
		std::vector<ScrollObject*> ScrollObjects;
	}
	bool CanRenderText = true;
	namespace FBO
	{
		unsigned int SSAOBuffers[3];
		unsigned int ssaoColorBuffer;
		unsigned int ssaoFBO;
	}
}


const bool IsInEditor = IS_IN_EDITOR;
const bool EngineDebug = ENGINE_DEBUG;

namespace Log
{
	std::vector<Message> Messages;
	void CreateNewLogMessage(std::string Text, Vector3 Color)
	{
		if (Messages.size() > 0)
		{
			if (Messages.at(Messages.size() - 1).Text == Text && Messages.at(Messages.size() - 1).Color == Color)
			{
				Messages.at(Messages.size() - 1).Ammount++;
			}
			else
			{
				Messages.push_back(Message(Text, Color));
			}
		}
		else
		{
			Messages.push_back(Message(Text, Color));
		}
		if (!IsInEditor)

		{
			std::cout << Text << "\n";
		}
	}
}

namespace Collision
{
	std::vector<CollisionComponent*> CollisionBoxes;
}

namespace Performance
{
	float DeltaTime;
	float FPS;
	float TimeMultiplier = 1;
	unsigned int DrawCalls = 0;
}

namespace Objects
{
	std::vector<WorldObject*> AllObjects;

	void CreateNewMeshObject(std::string FilePath, Transform Transform, std::string Name)
	{
		MeshObject* NewMeshObject = new MeshObject();
		NewMeshObject->LoadFromFile(FilePath);
		NewMeshObject->Start(Name, Transform);
		NewMeshObject->CurrentScene = World::CurrentScene;
	}
	std::vector<WorldObject*> GetAllObjectsWithID(uint32_t ID)
	{
		std::vector<WorldObject*> FoundObjects;
		for (WorldObject* o : Objects::AllObjects)
		{
			if (o->GetObjectDescription().ID == ID)
			{
				FoundObjects.push_back(o);
			}
		}
		return FoundObjects;
	}
}

namespace Stats
{
	float Time = 0;
}


namespace TextInput
{
	bool PollForText = false;
	std::string Text;
}

namespace Debugging
{
	std::string EngineStatus;
}

namespace Assets
{
	std::vector<Asset> Assets;

	void ScanForAssets(std::string Path, bool Recursive)
	{
		if (!Recursive)
		{
			if (!(IsInEditor || EngineDebug))
			{
				Path = "Assets/" + Path;
			}
		}
		if(!Recursive) Assets.clear();
		for (const auto& entry : std::filesystem::directory_iterator(Path))
		{
			if (entry.is_directory())
			{
				ScanForAssets(entry.path().string(), true);
			}
			else Assets.push_back(Asset(entry.path().string(), GetFileNameFromPath(entry.path().string())));
		}
	}


	std::string GetAsset(std::string Name)
	{
		for (const Asset& s : Assets::Assets)
		{
			if (s.Name == Name)
			{
				return s.Filepath;
			}
		}
		return "";
	}
}

namespace Editor
{
	bool IsInSubscene = false;
}