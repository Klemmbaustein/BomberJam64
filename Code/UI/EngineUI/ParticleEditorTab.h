#pragma once
#include <UI/EngineUI/EngineTab.h>
#include <UI/Default/TextRenderer.h>
#include <Rendering/Utility/Framebuffer.h>
#include <Rendering/Particle.h>
#include <World/Graphics.h>
#include <Rendering/Utility/ShaderManager.h>
#include <Math/Math.h>
#include <UI/Default/ScrollObject.h>

class ParticleEditorTab : public EngineTab
{
	std::vector<std::string> ElementMaterials = 
	{
	};
	const std::vector <std::string> ElementParametersColumn1 =
	{
		"Direction",
		"Direction Random",
		"Scale",
		"Lifetime",
		"Spawn Delay",
		"Num Loops",
		"Material"
	};
	const std::vector <std::string> ElementParametersColumn2 =
	{
		"Position Range",
		"Force",
		"Start Scale",
		"End Scale"
	}; 
	std::vector<UIBorder*> GeneratedUI;
	std::vector<UIBorder*> SettingsButtons;

	ScrollObject ElementScrollObject = ScrollObject(Vector2(0.3f, 0.65f), Vector2(0.4, 1.2), 15);
	TextRenderer* TabText = nullptr;
	FramebufferObject* ParticleFramebufferObject = nullptr;
	Particles::ParticleEmitter* Particle;
	std::string CurrentSystemFile;
	unsigned int SelectedElement = 0;
	float ReactivateDelay = 1.f;
public:
	ParticleEditorTab(TextRenderer* Text, GLuint SaveTexture, GLuint ReloadTexture);
	void Render(Shader* Shader) override;
	void Load(std::string File) override;
	void ReloadMesh();
	void Save() override;
	void Generate();
	void OnButtonClicked(int Index) override;
	virtual ~ParticleEditorTab();
};