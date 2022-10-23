#pragma once
#include <GL/glew.h>
#include <vector>
#include <Rendering/Renderable.h>
#include <Rendering/Particle.h>

class Framebuffer
{
public:

	void AttachFramebuffer(GLuint Buffer, GLuint Attachement = GL_COLOR_ATTACHMENT2)
	{
		Bind();
		GLuint* newArray = new GLuint[numbuffers + 1];
		for (unsigned int i = 0; i < numbuffers; i++)
		{
			newArray[i] = Textures[i];
		}
		newArray[numbuffers] = Buffer;
		numbuffers++;
		Textures = newArray;
		Attachements.push_back(Attachement);
		Unbind();
	}

	Framebuffer(int Width, int Height)
	{
	}

	~Framebuffer()
	{
		glDeleteFramebuffers(1, &FBO);
	}

	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	}

	void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	GLuint GetTextureID(int Index)
	{
		return Textures[Index];
	}

	void ReInit(int Width, int Height, bool ColorAttachementType = false)
	{
		if (FBO)
		{
			glDeleteFramebuffers(1, &FBO);
			glDeleteTextures(numbuffers, Textures);
		}

		glGenFramebuffers(1, &FBO);
		glGenTextures(numbuffers, Textures);
		for (unsigned int i = 0; i < numbuffers; i++)
		{
			Vector4 BorderColor = (Attachements[i] == GL_DEPTH_STENCIL_ATTACHMENT) ? Vector4(1) : Vector4(0, 0, 0, 1);

			glBindTexture(GL_TEXTURE_2D, Textures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, 
				(Attachements[i] == GL_DEPTH_STENCIL_ATTACHMENT) ? GL_DEPTH24_STENCIL8 : GL_RGBA16F, Width, Height, 0, (Attachements[i] == GL_DEPTH_STENCIL_ATTACHMENT) ? GL_DEPTH_STENCIL : GL_RGBA,
				Attachements[i] == GL_DEPTH_STENCIL_ATTACHMENT ? GL_UNSIGNED_INT_24_8 : GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &BorderColor.X);
		}

		Bind();
		for (unsigned int i = 0; i < numbuffers; i++)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, Attachements[i], GL_TEXTURE_2D, Textures[i], 0);
		}
		Unbind();
	}
	
protected:
	GLuint FBO = 0;
	unsigned int numbuffers = 3;
	GLuint* Textures = new GLuint[3];
	std::vector<GLuint> Attachements = {GL_COLOR_ATTACHMENT0, GL_DEPTH_STENCIL_ATTACHMENT, GL_COLOR_ATTACHMENT1};

};

class FramebufferObject
{
public:
	bool UseMainWindowResolution = true;
	Vector2 CustomFramebufferResolution = Vector2(800, 600);
	FramebufferObject();
	~FramebufferObject();

	unsigned int GetTextureID();
	void ClearContent();
	Camera* FramebufferCamera = nullptr;
	void ReInit();
	void UseWith(Renderable* r);
	std::vector<Particles::ParticleEmitter*> ParticleEmitters;
	Framebuffer* GetBuffer();
	std::vector<Renderable*> Renderables;
protected:
	Framebuffer* buf;
};