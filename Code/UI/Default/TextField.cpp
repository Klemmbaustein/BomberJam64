#include "UI/Default/UICanvas.h"
#include "TextField.h"
#include "WorldParameters.h"
#include "Math/Math.h"
#include <Timer.h>
#include <Input.h>
#include "VectorInputField.h"
#include <UI/Default/ScrollObject.h>
bool CurrentLineDisplayVisible = false;

bool TextFieldInitialized = false;

void ToggleCurrentLineDisplayVisible()
{
	CurrentLineDisplayVisible = !CurrentLineDisplayVisible;
	Timer::StartTimer(&ToggleCurrentLineDisplayVisible, 0.5f);
}

void TextField::Render(Shader* Shader)
{
	ScrollTick(Shader);
	if (TextFieldInitialized == false)
	{
		ToggleCurrentLineDisplayVisible();
		TextFieldInitialized = true;
	}
	Shader->Bind();
	Vector3 RenderColor;
	Vector2 PositionScreenSpace = Vector2(Position.X - 1.f / Graphics::WindowResolution.X);
	MeshVertexBuffer->Bind();
	Vector2 Offset;
	if (CurrentScrollObject != nullptr)
	{
		Offset.Y = CurrentScrollObject->Percentage;
	}
	glBindTexture(GL_TEXTURE_2D, TextureID);
	IsHovered = Maths::IsPointIn2DBox(Position + Offset, Vector2(Position.X + Scale.X, Position.Y + Scale.Y) + Offset, Input::MouseLocation);
	if (CurrentScrollObject != nullptr)
	{
		if (CurrentScrollObject->Position.Y + CurrentScrollObject->Scale.Y > Input::MouseLocation.Y)
		{
			IsHovered = false;
		}
		if (CurrentScrollObject->Position.Y < Input::MouseLocation.Y)
		{
			IsHovered = false;
		}
	}
	if (IsEdited)
	{
		if (TextInput::PollForText == true)
		{
			Text = TextInput::Text;
		}
		else
		{
			if (dynamic_cast<UICanvas*>(UI) != nullptr)
			{
				dynamic_cast<UICanvas*>(UI)->OnButtonClicked(ButtonIndex);
			}
			if (UI != nullptr)
			{
				if (dynamic_cast<VectorInputField*>(UI) != nullptr)
				{
					try
					{
						dynamic_cast<VectorInputField*>(UI)->OnEnteredValue(std::stof(Text), ButtonIndex);
					}
					catch (std::exception& e)
					{
					}
					return;
				}
			}
			IsEdited = false;
		}
	}
	if (UseTexture)
		glUniform1i(glGetUniformLocation(Shader->GetShaderID(), "u_usetexture"), 1);
	else
		glUniform1i(glGetUniformLocation(Shader->GetShaderID(), "u_usetexture"), 0);
	if (!IsHovered)
	{
		RenderColor = Color;
		if (Input::IsLMBDown)
		{
			if (IsEdited == true)
			{
				if (dynamic_cast<UICanvas*>(UI) != nullptr)
				{
					dynamic_cast<UICanvas*>(UI)->OnButtonClicked(ButtonIndex);
				}
				if (dynamic_cast<VectorInputField*>(UI) != nullptr)
				{
					try
					{
						dynamic_cast<VectorInputField*>(UI)->OnEnteredValue(std::stof(Text), ButtonIndex);
					}
					catch (std::exception& e)
					{
					}
					return;
				}
				IsEdited = false;

			}
		}
	}
	if (IsEdited)
	{
		RenderColor = Vector3(Color.X * 0.8f, Color.Y * 0.8f, Color.Z * 0.8f);
	}
	if (!Input::IsLMBDown && IsHovered)
	{
		RenderColor = Vector3(Color.X * 0.8f, Color.Y * 0.8f, Color.Z * 0.8f);
		if (IsButtonDown)
		{
			TextInput::PollForText = true;
			IsButtonDown = false;
			TextInput::Text = Text;
			Selected = false;
			IsEdited = true;
		}
	}
	else if(Input::IsLMBDown && IsHovered)
	{
		RenderColor = Vector3(Color.X * 0.5f, Color.Y * 0.5f, Color.Z * 0.5f);
		if (!IsButtonDown)
		{
			IsButtonDown = true;
		}
	}
	glUniform4f(glGetUniformLocation(Shader->GetShaderID(), "u_color"), RenderColor.X, RenderColor.Y, RenderColor.Z, 1.f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	MeshVertexBuffer->Unbind();
	int size = Scale.X * 100.f / TextSize;
	if (size != 0)
	{
		for (int i = 0; i < std::truncf(Text.size() / size) + 1; i++)
		{
			Renderer->RenderText(Text.substr(i * size, i * size + size < Text.size() ? size : Text.size()) + (IsEdited && i >= std::truncf(Text.size() / size) && CurrentLineDisplayVisible ? "_" : ""), Position + Vector2(0.0025f, (i * -0.035f) + Scale.Y - 0.04f), 1 * TextSize, Vector3(1.f), CurrentScrollObject);
		}
	}
}