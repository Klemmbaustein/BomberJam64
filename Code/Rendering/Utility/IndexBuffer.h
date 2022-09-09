#pragma once

#include <GL/glew.h>

#include "Rendering/Vertex.h"

struct IndexBuffer
{
	IndexBuffer(void* data, int NumVertecies, int ElementSize)
	{ 
		glGenBuffers(1, &BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumVertecies * ElementSize, data, GL_STATIC_DRAW);
	}

	virtual ~IndexBuffer()
	{
		glDeleteBuffers(1, &BufferID);
	}

	void Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferID);
	}

	void Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


private:
	GLuint BufferID;
};