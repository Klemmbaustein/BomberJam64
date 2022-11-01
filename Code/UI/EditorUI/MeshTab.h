#pragma once
#include "EditorTab.h"
#include <UI/Default/TextField.h>
#include "MaterialFunctions.h"


class MeshTab : public EngineTab
{
public:
	MeshTab(TextRenderer* Renderer);
	void Render(Shader* Shader) override;
	void Load(std::string File) override;
	void ReloadMesh();
	void Save() override;
	void Generate();
	void OnButtonClicked(int Index) override;
	virtual ~MeshTab();
protected:
	UIButton* CastShadowButton = new UIButton(Vector2(-0.465f, 0.425f), Vector2(0.05f, 0.05f), Vector3(0.2f, 0.2f, 0.32f), this, 0);
	UIButton* HasCollisionButton = new UIButton(Vector2(-0.465f, 0.37f), Vector2(0.05f, 0.05f), Vector3(0.2f, 0.2f, 0.32f), this, -2);
	UIButton* TwoSidedButton = new UIButton(Vector2(-0.465f, 0.315f), Vector2(0.05f, 0.05f), Vector3(0.2f, 0.2f, 0.32f), this, -3);

	TextRenderer* Renderer;
	int NumVertices;
	int MaterialSlots;
	std::vector<std::string> Materials;
	std::vector<TextField*> TextFields;
	std::string Filepath;
	std::string InitialName;
};