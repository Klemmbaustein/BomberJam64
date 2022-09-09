#pragma once
#include <UI/Default/UICanvas.h>
#include <UI/Default/TextField.h>
#include <UI/EngineUI/EngineTab.h>
#include "MaterialTemplate.h"
#include <UI/Default/VectorInputField.h>

class MaterialUI : public EngineTab
{
public:
	virtual void OnButtonClicked(int Index) override;

	MaterialUI(Vector2 Position, Vector2 Scale, Vector3 Color, TextRenderer* Text, GLuint SaveTexture, GLuint ReloadTexture) : EngineTab(Position, Scale, Color)
	{
		UIElements.push_back(new UIBorder(Vector2(-0.7f, -0.6f), Vector2(1.4, 1.28f), Vector3(0.1f, 0.1f, 0.105f)));
		Renderer = Text;
		ShaderFileTextFields = {new TextField(Vector2(-0.6f, 0.4f), Vector2(0.4f, 0.075f), Vector3(0.3f), Renderer, this, -2)};
		UIElements.push_back(ShaderFileTextFields.at(0));
		UIElements.push_back(new UIButton(Vector2(0.375, 0.55), Vector2(0.045f, 0.08), Vector3(1), this, -4, false, ReloadTexture, true, true));
		UIElements.push_back(IsTransparentButton);

		GenerateUI();
	}

	void Render(Shader* Shader) override; //Render and Update
	void Load(std::string File) override;
	void LoadTemplate(std::string Template);
	void FetchTemplate(std::string Template);
	void Save();
	void GenerateUI();
private:
	TextRenderer* Renderer;
	std::string Filepath, FragmentShader, VertexShader;
	std::vector<TextField*> ShaderFileTextFields;
	std::string ParentTemplate;
	std::vector<MaterialTemplates::TemplateParam> Paramters;
	std::vector<UIBorder*> TextFields;
	UIButton* IsTransparentButton = new UIButton(Vector2(0.142f, 0.38f), Vector2(0.05f, 0.05f), Vector3(1), this, -5);
	bool IsTransparent = false;
};