#include "UIButton.h"
#include "Math/Math.h"
#include "UICanvas.h"
#include <UI/Default/ScrollObject.h>
#include <Engine/Input.h>
#include <World/Graphics.h>

extern bool UserDraggingButton;
void UIButton::Render(Shader* Shader)
{
	Shader->Bind();
	Vector3 RenderColor;
	Vector2 PositionScreenSpace = Vector2(Position.X - 1.f / Graphics::WindowResolution.X);
	MeshVertexBuffer->Bind();
	ScrollTick(Shader);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	Vector2 Offset;
	if (CurrentScrollObject != nullptr)
	{
		Offset.Y = CurrentScrollObject->Percentage;
	}
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
	if (UseTexture)
		glUniform1i(glGetUniformLocation(Shader->GetShaderID(), "u_usetexture"), 1);
	else
		glUniform1i(glGetUniformLocation(Shader->GetShaderID(), "u_usetexture"), 0);
	if (!IsHovered)
	{
		RenderColor = Color;
		if (Input::IsLMBDown && IsButtonDown)
		{
			if (CanBeDragged)
			{
				dynamic_cast<UICanvas*>(UI)->OnButtonDragged(ButtonIndex);
				Selected = false;

			}
			IsButtonDown = false;
		}
	}
	else if (!Input::IsLMBDown)
	{
		RenderColor = Vector3(Color.X * 0.8f, Color.Y * 0.8f, Color.Z * 0.8f);
		if (IsButtonDown)
		{
			if (!NeedsToBeSelected || Selected || UserDraggingButton)
			{
				ButtonEvents.push_back(ButtonEvent(dynamic_cast<UICanvas*>(UI), ButtonIndex));
				IsButtonDown = false;
				Selected = false;

			}
			else
			{
				Selected = true;
				IsButtonDown = false; 
			}
		}
	}
	else
	{
		RenderColor = Vector3(Color.X * 0.5f, Color.Y * 0.5f, Color.Z * 0.5f);
		if (!IsButtonDown)
		{
			IsButtonDown = true;
		}
	}
	
	if (Selected)
	{
		RenderColor = Vector3(Color.X * 0.8f, Color.Y * 0.8f, Color.Z * 2.f);
		if (IsButtonDown)
		{
			RenderColor = Vector3(Color.X * 0.5f, Color.Y * 0.5f, Color.Z * 1.5f);
		}
		else if (Input::IsLMBDown)
			Selected = false;
		else if (IsHovered)
			RenderColor = Vector3(Color.X * 0.75f, Color.Y * 0.75f, Color.Z * 1.5f);

	}

	glUniform1f(glGetUniformLocation(Shader->GetShaderID(), "u_opacity"), Opacity);
	glUniform4f(glGetUniformLocation(Shader->GetShaderID(), "u_color"), RenderColor.X, RenderColor.Y, RenderColor.Z, 1.f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	MeshVertexBuffer->Unbind();
}
