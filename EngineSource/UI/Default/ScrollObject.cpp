#include <UI/Default/ScrollObject.h>
#include <Math/Math.h>
#include <World/Graphics.h>
#include <Engine/Input.h>


ScrollObject::ScrollObject(Vector2 Position, Vector2 Scale, float MaxScroll)
{
	this->Position = Position;
	this->Scale = Vector2() - Scale;
	Graphics::UI::ScrollObjects.push_back(this);
	this->MaxScroll = MaxScroll;
}

ScrollObject::~ScrollObject()
{
	int i = 0;
	for (ScrollObject* s : Graphics::UI::ScrollObjects)
	{
		if (s == this)
		{
			Graphics::UI::ScrollObjects.erase(Graphics::UI::ScrollObjects.begin() + i);
		}
		i++;
	}
}

void ScrollObject::ScrollUp()
{
	if (Maths::IsPointIn2DBox(Position, Position + Scale, Input::MouseLocation))
	{
		Percentage += Speed / 100.f;
	}
	if (Percentage > MaxScroll / 10)
	{
		Percentage = MaxScroll / 10;
	}
}

void ScrollObject::ScrollDown()
{
	if (Maths::IsPointIn2DBox(Position, Position + Scale, Input::MouseLocation))
	Percentage -= Speed / 100.f;
	if (Percentage < 0)
		Percentage = 0;
}
