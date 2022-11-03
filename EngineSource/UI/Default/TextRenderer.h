#pragma once
#include <GL/glew.h>
#include <Math/Vector.h>
#include "glm/ext.hpp"

void OnWindowResized();
struct FontVertex {
	glm::vec2 position;
	glm::vec2 texCoords;
};
class ScrollObject;

class TextRenderer
{
private:
	void* cdatapointer;
	GLuint fontTexture;
	GLuint fontVao;
	GLuint fontVertexBufferId;
	FontVertex* fontVertexBufferData = 0;
	uint32_t fontVertexBufferCapacity;
	std::string Filename; float CharacterSizeInPixels;
	glm::mat4 projection = glm::ortho(-800.0f, 800.0f, 450.0f, -450.f);
public:
	TextRenderer(std::string filename = "Fonts/Font.ttf", float CharacterSizeInPixels = 30);

	void RenderText(std::string Text, Vector2 Pos, float scale, Vector3 color, ScrollObject* CurrentScrollObject = nullptr);
	void Reinit();
	~TextRenderer();

};