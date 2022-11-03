#pragma once
#include "UIBorder.h"
#include "UIButton.h"

class UIRenderingException : public std::exception
{
public:
    UIRenderingException(std::string UIType, std::string ErrorType)
    {
        Exception = "UIException thrown in " + UIType + ": " + ErrorType;
    }

    virtual const char* what() const throw()
    {
        return Exception.c_str();
    }

    std::string Exception;
};

class UICanvas : public UIBorder
{
public:
    UICanvas() : UIBorder(Vector2(), Vector2(), Vector3())
    {
        //Insert all UI Elements in UIElements
    }

    virtual void OnButtonClicked(int Index);

    virtual void OnButtonDragged(int Index);
    void _RenderUIElements(Shader* _Shader);
    virtual ~UICanvas();
protected:
    std::vector<UIBorder*> UIElements;
};
struct ButtonEvent
{
    UICanvas* c;
    int Index;
};

extern std::vector<ButtonEvent> ButtonEvents;