#pragma once
#include <UI/EngineUI/EngineTab.h>
#include <UI/Default/TextField.h>

class EngineUI;

class AddNewItemTab : public EngineTab
{
public:
	void Render(Shader* Shader) override;
	AddNewItemTab(TextRenderer* Renderer, EngineUI* Parent);
	~AddNewItemTab();
	void Load(std::string Filepath) override;
	void Save() override;
	void OnButtonClicked(int Index) override;
private:
	TextRenderer* Renderer;
	TextField* MainTextField;
	std::string FileToAdd;
	EngineUI* ParentUI;
};