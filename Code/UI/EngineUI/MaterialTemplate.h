#pragma once
#include <UI/Default/UICanvas.h>
#include <UI/Default/TextField.h>
#include <UI/EngineUI/EngineTab.h>

namespace MaterialTemplates
{
	struct TemplateParam
	{
		std::string UniformName;
		int Type;
		std::string Value;
		TemplateParam(std::string UniformName, int Type, std::string Value)
		{
			this->UniformName = UniformName;
			this->Type = Type;
			this->Value = Value;
		}
	};
}

class MaterialTemplate : public EngineTab
{
public:
	virtual void OnButtonClicked(int Index) override;

	MaterialTemplate(Vector2 Position, Vector2 Scale, Vector3 Color, TextRenderer* Text, GLuint SaveTexture) : EngineTab(Position, Scale, Color)
	{
		UIElements.push_back(new UIBorder(Vector2(-0.7f, -0.6f), Vector2(1.4, 1.28f), Vector3(0.1f, 0.1f, 0.105f)));
		Renderer = Text;
		ShaderFileTextFields = { new TextField(Vector2(-0.6f, 0.4f), Vector2(0.4f, 0.075f), Vector3(0.3f), Renderer, this, -2),
		new TextField(Vector2(-0.1f, 0.4f), Vector2(0.4f, 0.075f), Vector3(0.3f), Renderer, this, -2) };
		UIElements.push_back(new UIButton(Vector2(-0.5f, 0.28f), Vector2(0.12, 0.07), Vector3(0.7f), this, -1));
		UIElements.push_back(ShaderFileTextFields.at(0));
		UIElements.push_back(ShaderFileTextFields.at(1));
		UIElements.push_back(new UIButton(Vector2(0.4, 0.55), Vector2(0.045f, 0.08), Vector3(1), this, -3, false, SaveTexture, true, true));

		GenerateUI();
	}

	void Render(Shader* Shader) override; //Render and Update
	void Load(std::string File) override;
	void Save() override;
	void GenerateUI();
private:
	TextRenderer* Renderer;
	std::string Filepath;
	std::vector<TextField*> ShaderFileTextFields;
	std::string VertexShader, FragmentShader;
	std::vector<MaterialTemplates::TemplateParam> Parameters;
	std::vector<UIButton*> RemoveButtons;
	std::vector<UIBorder*> TextFields;
};