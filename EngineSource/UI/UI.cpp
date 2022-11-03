#include "UI.h"
#include <Rendering/VertexBuffer.h>
#include <Rendering/Utility/IndexBuffer.h>
#include <Rendering/Shader.h>
namespace UI
{
	VertexBuffer* v;
	IndexBuffer* i;
}
namespace Graphics
{
	extern Shader* UIShader;
}

//TODO: #2