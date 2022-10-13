#pragma once
#include <Math/Vector.h>
#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif
#include <SDL.h>

namespace Input
{
	bool IsKeyDown(int Key);

	extern Vector2 MouseMovement;
	extern bool IsLMBDown;
	extern bool IsRMBDown;
	extern bool CursorVisible;
	extern Vector2 MouseLocation;
}

namespace TextInput
{
	extern bool PollForText;
	extern std::string Text;
}