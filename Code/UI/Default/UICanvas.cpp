#include "UICanvas.h"

void UICanvas::OnButtonClicked(int Index)
{

}

void UICanvas::OnButtonDragged(int Index)
{
}

void UICanvas::_RenderUIElements(Shader* _Shader)
{
	for (int i = 0; i < UIElements.size(); i++)
	{
		if (UIElements.size() > i)
		UIElements[i]->Render(_Shader);
	}
}

UICanvas::~UICanvas()
{
	for (UIBorder* c : UIElements)
	{
		delete c;
	}
}
