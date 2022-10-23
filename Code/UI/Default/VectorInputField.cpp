#include "VectorInputField.h"
#include "UICanvas.h"

const std::string Coords[] = { "X", "Y", "Z" };

void VectorInputField::Render(Shader* Shader)
{
	ScrollTick(Shader);
	for (UIBorder* t : TextFields)
	{
		t->CurrentScrollObject = CurrentScrollObject;
		t->Render(Shader);
	}
	for (int i = 0; i < 3; i++)
	{
		TextRenderer->RenderText(Coords[i], Position + Vector2(((float)i / 10) - 0.03f, 0.f) + Vector2(0.0075f, 0.02), 1.8f, Vector3(1), CurrentScrollObject);
	}
}

Vector3 VectorInputField::GetValue()
{
	return Value;
}

void VectorInputField::SetValue(Vector3 In)
{
	for (UIBorder* t : TextFields)
	{
		delete t;
	}
	TextFields.clear();
	Value = In;

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

void VectorInputField::OnEnteredValue(float Value, int Index)
{
	switch (Index)
	{
	case 0:
		this->Value.X = Value;
		break;
	case 1:
		this->Value.Y = Value;
		break;
	case 2:
		this->Value.Z = Value;
		break;
	default:
		break;
	}
	if (dynamic_cast<UICanvas*>(ParentUI) != nullptr)
	{
		dynamic_cast<UICanvas*>(ParentUI)->OnButtonClicked(this->Index);
	}
	SetValue(this->Value);
}
