#pragma once
#include "GL/glew.h"
#include <string>
#include <iostream>
struct Shader
{
	Shader(const char* VertexShaderFilename, const char* FragmentShaderFilename, const char* GeometryShader = nullptr);
	virtual ~Shader();

	void Bind();
	void Unbind();

	int GetShaderID()
	{
		return ShaderID;
	}


	void Recompile();

private:
	GLuint Compile(std::string ShaderCode, GLenum Type);
	std::string parse(const char* Filename);
	GLuint CreateShader(const char* VertexShader, const char* FragmentShader, const char* GeometryShader);
	void checkCompileErrors(GLuint shader, std::string type, std::string ShaderName);

	std::string VertexFileName, FragmetFileName;

	GLuint ShaderID;
};