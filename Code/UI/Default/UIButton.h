#pragma once
#include "UIBorder.h"

class UIButton : public UIBorder
{
public:
	virtual void Render(Shader* Shader) override;

	UIButton(Vector2 Position, Vector2 Scale, Vector3 Color, UIBorder* Parent, int Index, bool CanBeDragged = false,
		GLuint TextureID = (unsigned int)0, bool UseTexture = false, bool FlipTexture = false) :
		UIBorder(Position, Scale, Color, TextureID, UseTexture, FlipTexture)
	{
		ButtonIndex = Index;
		this->UI = Parent;
		this->CanBeDragged = CanBeDragged;
	}
	bool NeedsToBeSelected = false; //Basically double clicking, like content browser buttons
	bool Selected = false;
	bool IsButtonDown = false;
	bool IsHovered = false;
protected:
	UIBorder* UI = nullptr;
	int ButtonIndex = -1;
	bool CanBeDragged = false;
};