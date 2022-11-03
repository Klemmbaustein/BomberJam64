#pragma once
#include <Rendering/Camera/Camera.h>
#include <Rendering/Shader.h>
class Renderable
{
public:
	virtual void Render(Camera* WorldCamera) = 0;
	virtual void SimpleRender(Shader* Shader) = 0;
	Renderable()
	{

	};
	virtual ~Renderable()
	{

	};
	bool CastShadow = true;
};