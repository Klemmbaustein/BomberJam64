#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <Rendering/Renderable.h>
#include <vector>
#include "Rendering/Shader.h"

class CollisionComponent;
class ScrollObject;
class UICanvas;
class WorldObject;

namespace Engine
{
	extern std::string CurrentProjectName;
	extern std::string VersionString;
}

namespace Graphics
{
	struct Fog
	{
		float Distance = 70.f;
		float Falloff = 500.f;
		float MaxDensity = 1.f;
		Vector3 FogColor = Vector3(0.7f, 0.8f, 1.f);
	};
	struct Sun
	{
		float Intensity = 1.f;
		float AmbientIntensity = 0.3f;
		Vector3 Direction = Vector3(0.5f, 0.5f, 0.7f);
		Vector3 SunColor = Vector3(1.f, 0.9f, 0.9f);
		Vector3 AmbientColor = Vector3(0.5f, 0.5f, 1.f);
	};
	extern Sun WorldSun;
	extern Fog WorldFog;
	extern unsigned int ShadowResolution;
	extern std::vector<Renderable*> ModelsToRender;
	extern std::vector<UICanvas*> UIToRender;
	extern Vector2 WindowResolution;
	void SetWindowResolution(Vector2 NewResolution);
	extern Camera* MainCamera;
	extern Shader* MainShader;
	extern Shader* ShadowShader;
	extern Shader* TextShader;
	extern bool IsRenderingShadows;
	extern bool Bloom, FXAA;
	extern float Gamma;
	extern float ChrAbbSize, Vignette;
	extern Shader* UIShader;
	extern std::vector<CollisionComponent*> Objects;
	extern unsigned int PCFQuality;
	extern Vector3 LightRotation;
	namespace UI
	{
		extern std::vector<ScrollObject*> ScrollObjects;
	}
	void RecompileShaders();
	namespace FBO
	{
		extern unsigned int SSAOBuffers[3];
		extern unsigned int ssaoColorBuffer;
		extern unsigned int ssaoFBO;

	}
}

extern float Time;


namespace Performance
{
	extern float DeltaTime;
	extern float FPS;
	extern float TimeMultiplier;
	extern unsigned int DrawCalls;
}

namespace Log
{
	struct Message
	{
		Vector3 Color;
		std::string Text;
		int Ammount = 0;
		Message(std::string Text, Vector3 Color)
		{
			this->Color = Color;
			this->Text = Text;
		}
	};
	extern std::vector<Message> Messages;
}

const extern bool IsInEditor;
const extern bool EngineDebug;
namespace Objects
{
	extern std::vector<WorldObject*> AllObjects;
	void CreateNewMeshObject(std::string FilePath, Transform Transform, std::string Name = "NONE");
	std::vector<WorldObject*> GetAllObjectsWithID(uint32_t ID);
}

namespace Collision
{
	extern std::vector<CollisionComponent*> CollisionBoxes;
}

namespace Input
{
	extern bool Keys[351];
	extern bool CursorVisible;
	extern Vector2 MouseLocation;
}

namespace TextInput
{
	extern bool PollForText;
	extern std::string Text;
}

namespace Debugging
{
	extern std::string EngineStatus;
}

namespace Assets
{
	struct Asset
	{
		Asset(std::string Filepath, std::string Name)
		{
			this->Name = Name;
			this->Filepath = Filepath;
		}
		std::string Filepath;
		std::string Name;
	};
	extern std::vector<Asset> Assets;

	void ScanForAssets(std::string Path = "Content/", bool Recursive = false);
	std::string GetAsset(std::string Name);
}

namespace Editor
{
	extern bool IsInSubscene;
}