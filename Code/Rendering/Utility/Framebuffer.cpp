#include "Framebuffer.h"
#include <World/Graphics.h>

FramebufferObject::FramebufferObject()
{
	buf = new Framebuffer(Graphics::WindowResolution.X, Graphics::WindowResolution.Y);
	Graphics::AllFramebuffers.push_back(this);
}

FramebufferObject::~FramebufferObject()
{
	for (unsigned int i = 0; i < Graphics::AllFramebuffers.size(); i++)
	{
		if (Graphics::AllFramebuffers[i] == this)
		{
			Graphics::AllFramebuffers.erase(Graphics::AllFramebuffers.begin() + i);
		}
	}
	delete buf;
}

unsigned int FramebufferObject::GetTextureID()
{
	return buf->GetTextureID(0);
}

void FramebufferObject::ReInit()
{
	if (!UseMainWindowResolution)
	{
		buf->ReInit(CustomFramebufferResolution.X, CustomFramebufferResolution.Y);
		FramebufferCamera->ReInit(FramebufferCamera->FOV, CustomFramebufferResolution.X, CustomFramebufferResolution.Y);
	}
	else
	{
		buf->ReInit(Graphics::WindowResolution.X, Graphics::WindowResolution.Y);
		FramebufferCamera->ReInit(FramebufferCamera->FOV, Graphics::WindowResolution.X, Graphics::WindowResolution.X);
	}
}

void FramebufferObject::UseWith(Renderable* r)
{
	Renderables.push_back(r);
}

Framebuffer* FramebufferObject::GetBuffer()
{
	return buf;
}

void FramebufferObject::ClearContent()
{
	for (Renderable* r : Renderables)
	{
		delete r;
	}
	Renderables.clear();
	ParticleEmitters.clear();
}