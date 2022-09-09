#pragma once
#include "TextField.h"
#include <iomanip>
#include <sstream>

class VectorInputField : public UIBorder
{
public:
	VectorInputField(Vector2 Position, Vector2 Scale, Vector3 Color, int Index, TextRenderer* TextRenderer, UIBorder* ParentUI, Vector3 StartValue = Vector3()) : UIBorder(Position, Scale, Color)
	{
		Position += Vector2(0.03f, 0.f);
		Value = StartValue;
		this->Index = Index;
		this->TextRenderer = TextRenderer;
		this->ParentUI = ParentUI;
		this->Position = Position;
		for (int i = 0; i < 3; i++)
		{
			TextFields.push_back(new TextField(Position + Vector2((float)i / 10, 0), Vector2(0.08f, 0.08f), Vector3(0.25), TextRenderer, this, i, 0.9f));
			std::stringstream stream;
			stream << std::fixed << std::setprecision(3) << Value.at(i);
			std::string s = stream.str();
			dynamic_cast<TextField*>(TextFields.at(TextFields.size() - 1))->Text = s;
			Vector3 InputColor;
			switch (i)
			{
			case 0:
				InputColor = Vector3(0.8f, 0.f, 0.f);
				break;
			case 1:
				InputColor = Vector3(0.f, 0.8f, 0.f);
				break;
			case 2:
				InputColor = Vector3(0.f, 0.f, 0.8f);
				break;
			default:
				break;
			}
			TextFields.push_back(new UIBorder(Position + Vector2(((float)i / 10) - 0.03f, 0), Vector2(0.03f, 0.08f), InputColor));
		}
	}
	virtual ~VectorInputField()
	{
		for (UIBorder* t : TextFields)
		{
			delete t;
		}
		TextFields.clear();
	}

	void Render(Shader* Shader) override;

	Vector3 GetValue();
	void SetValue(Vector3 In);
protected:
	void OnEnteredValue(float Value, int Index);
	friend TextField;
	std::vector<UIBorder*> TextFields;
	Vector3 Value;
	UIBorder* ParentUI;
	TextRenderer* TextRenderer;
	int Index;
};