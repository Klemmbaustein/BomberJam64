#pragma once
#include "UI/Default/UIBorder.h"
#include "UI/Default/UICanvas.h"

class EngineTab : public UICanvas
{
public:
	EngineTab(Vector2 Position, Vector2 Scale, Vector3 Color) : UICanvas()
	{
		//Insert all UI Elements in UIElements
	}

	virtual void Load(std::string File) = 0;

	virtual void Save() = 0;

protected:
	std::vector<UIBorder*> UIElements;
};