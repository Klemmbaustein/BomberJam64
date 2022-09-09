#include "WorldParameters.h"
#include "Input.h"

bool Input::IsKeyDown(int Key)
{
	if (!(Key < 128))
	{
		Key -= 1073741755;
	}
	bool Test = Input::Keys[Key];
	return Test;
}

namespace Input
{
	bool IsLMBDown = false;
	bool IsRMBDown = false;
}

Vector2 Input::MouseMovement;