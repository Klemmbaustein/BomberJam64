#include "Texture.h"
#include <Utility/stb_image.h>
#include <GL/glew.h>
#include <Engine/Log.h>

namespace Assets
{
	std::string GetAsset(std::string File);
}

namespace Texture
{
	std::vector<Texture> Textures;

	unsigned int LoadTexture(std::string TexturePath)
	{
		for (Texture& t : Textures)
		{
			if (t.TexturePath == TexturePath)
			{
				++t.References;
				return t.TextureID;
			}
		}
		try
		{
			int TextureWidth = 0;
			int TextureHeigth = 0;
			int BitsPerPixel = 0;
			stbi_set_flip_vertically_on_load(true);
			auto TextureBuffer = stbi_load((Assets::GetAsset(TexturePath)).c_str(), &TextureWidth, &TextureHeigth, &BitsPerPixel, 4);
			GLuint TextureID;
			glGenTextures(1, &TextureID);
			glBindTexture(GL_TEXTURE_2D, TextureID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TextureWidth, TextureHeigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureBuffer);

			Textures.push_back(Texture(TextureID, 1, TexturePath));
			if (TextureBuffer)
			{
				stbi_image_free(TextureBuffer);
			}
			return TextureID;
		}
		catch (std::exception& e)
		{
			Log::CreateNewLogMessage(std::string("Error loading Texture: ") + e.what(), Vector3(0.7f, 0.f, 0.f));
			return 0;
		}
	}

	void UnloadTexture(unsigned int TextureID)
	{
		for (int i = 0; i < Textures.size(); i++)
		{
			Texture& t = Textures.at(i);
			if (t.TextureID == TextureID)
			{
				--t.References;
				if (t.References <= 0)
				{
					glDeleteTextures(1, &Textures.at(i).TextureID);
					Textures.erase(Textures.begin() + i);
				}
				return;
			}
		}
		Log::CreateNewLogMessage("Error unloading texture: " + std::to_string(TextureID) + ", (Texture not loaded)", Vector3(0.7f, 0.f, 0.f));
	}
}