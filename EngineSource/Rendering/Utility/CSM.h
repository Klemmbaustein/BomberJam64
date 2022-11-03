#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <GL/glew.h>

namespace CSM
{

	struct CSMInitExeption : std::exception
	{
		CSMInitExeption(std::string ErrorMessage)
		{
			this->ErrorMessage = ErrorMessage;
		}
		std::string ErrorMessage;

		const char* what() const noexcept 
		{
			return ErrorMessage.c_str();
		}
	};
	extern const float cameraFarPlane;
	extern std::vector<float> shadowCascadeLevels;
	extern GLuint LightFBO;
	extern int Cascades;
	extern GLuint ShadowMaps;

	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);

	std::string ErrorMessageFromGLStatus(int Status);

	std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
	void Init();
	void ReInit();
	glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane);

	std::vector<glm::mat4> getLightSpaceMatrices();
}