#pragma once
#include <vector>
class UICanvas;
const extern bool IsInEditor;
namespace Graphics
{
	extern std::vector<UICanvas*> UIToRender;
}
namespace UI
{
	template<typename T>
	inline T* CreateUICanvas()
	{
		if (IsInEditor)
		{
			return nullptr;
		}
		else
		{
			T* newT = new T();
			Graphics::UIToRender.push_back(newT);
			return newT;
		}
	}
}