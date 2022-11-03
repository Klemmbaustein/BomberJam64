#pragma once
#include <UI/Default/UIButton.h>
#include <UI/Default/TextRenderer.h>
class TextField : public UIBorder
{
public:
	TextField(Vector2 Position, Vector2 Scale, Vector3 Color, TextRenderer* Renderer, UIBorder* ParentUI, int Index, float TextSize = 1.f, GLuint TextureID = (unsigned int)0, bool UseTexture = false) : UIBorder(Position, Scale, Color, TextureID, UseTexture)
	{
		this->Renderer = Renderer;
		UI = ParentUI;
		this->ButtonIndex = Index;
		this->TextSize = TextSize;
	}

	virtual void Render(Shader* Shader) override;
	TextRenderer* Renderer;
	bool NeedsToBeSelected = false;
	bool Selected = false;
	bool IsButtonDown = false;
	bool IsHovered = false;
	std::string Text;
protected:
	float TextSize;
	UIBorder* UI = nullptr;
	int ButtonIndex = -1;
	bool CanBeDragged = false;
	bool IsEdited = false;
};