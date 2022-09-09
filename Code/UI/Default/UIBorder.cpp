#include "UIBorder.h"
#include "UICanvas.h"

#include <UI/Default/ScrollObject.h>
void UIBorder::Render(Shader* Shader)
{
	Shader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	MeshVertexBuffer->Bind();
	ScrollTick(Shader);
	glUniform4f(glGetUniformLocation(Shader->GetShaderID(), "u_color"), Color.X, Color.Y, Color.Z, 1.f);

	if(UseTexture)
		glUniform1i(glGetUniformLocation(Shader->GetShaderID(), "u_usetexture"), 1);
	else
		glUniform1i(glGetUniformLocation(Shader->GetShaderID(), "u_usetexture"), 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	MeshVertexBuffer->Unbind();
}

UIBorder::~UIBorder()
{
	delete MeshVertexBuffer;
}

void UIBorder::ScrollTick(Shader* UsedShader)
{
	if (CurrentScrollObject != nullptr)
	{
		glUniform3f(glGetUniformLocation(UsedShader->GetShaderID(), "u_offset"), -CurrentScrollObject->Percentage, CurrentScrollObject->Position.Y + CurrentScrollObject->Scale.Y, CurrentScrollObject->Position.Y);
	}
	else
	glUniform3f(glGetUniformLocation(UsedShader->GetShaderID(), "u_offset"), 0, -1000, 1000);

}
