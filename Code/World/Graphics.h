#pragma once
#include <Math/Vector.h>

class Renderable;
class UICanvas;
class Camera;
class Shader;
class ScrollObject;

namespace Graphics
{
	struct Fog
	{
		float Distance = 70.f;
		float Falloff = 500.f;
		float MaxDensity = 1.f;
		Vector3 FogColor = Vector3(0.8f, 0.9f, 1.f);
	};
	struct Sun
	{
		float Intensity = 1.f;
		float AmbientIntensity = 0.3f;
		Vector3 Direction = Vector3(0.5f, 0.5f, 0.7f);
		Vector3 SunColor = Vector3(1.2f, 1.2f, 0.9f);
		Vector3 AmbientColor = Vector3(0.7f, 0.7f, 1.f);
	};

	struct Light
	{
		Light(float Intensity = 1.f, float Range = 1.f, Vector3 Color = 1.f)
		{
			this->Intensity = Intensity;
			this->Range = Range;
			this->Color = Color;
		}

		float Intensity = 1.f;
		float Range = 1.f;
		Vector3 Color = 1.f;
	};

	extern std::vector<Light> Lights;
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