#include "SSAO.h"
#include <random>
#include <vector>
#include <glm/glm.hpp>
#include <Rendering/Utility/ShaderManager.h>
#include <WorldParameters.h>
#define GLEW_STATIC
#include <GL/glew.h>
std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
std::default_random_engine generator;
std::vector<glm::vec3> ssaoKernel;
std::vector<glm::vec3> ssaoNoise;
Shader* AOShader;
unsigned int noiseTexture;
unsigned int ssaoFBO = 0;
unsigned int ssaoColorBuffer;
unsigned int ssaoBlurFBO, ssaoColorBufferBlur;
unsigned int ResolutionDivider = 1;
unsigned int Samples = 16;
unsigned int SSAOTexture;
void SSAO::Init()
{

	for (unsigned int i = 0; i < Samples; ++i)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = (float)i / Samples;
		scale = std::lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	AOShader = ReferenceShader("Shaders/postprocess.vert", "Shaders/ssao.frag");
}

unsigned int SSAO::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Graphics::FBO::SSAOBuffers[1]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Graphics::FBO::SSAOBuffers[2]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	AOShader->Bind();
	glUniform1i(glGetUniformLocation(AOShader->GetShaderID(), "FullScreen"), 1);
	glUniform1i(glGetUniformLocation(AOShader->GetShaderID(), "kernelSize"), Samples);
	glUniform1f(glGetUniformLocation(AOShader->GetShaderID(), "radius"), 5.5f);
	glUniform1i(glGetUniformLocation(AOShader->GetShaderID(), "gPosition"), 0);
	glUniform1i(glGetUniformLocation(AOShader->GetShaderID(), "gNormal"), 1);
	glUniform1i(glGetUniformLocation(AOShader->GetShaderID(), "ResDiv"), ResolutionDivider);
	glUniform1i(glGetUniformLocation(AOShader->GetShaderID(), "texNoise"), 2);
	glUniform2f(glGetUniformLocation(AOShader->GetShaderID(), "screenRes"), Graphics::WindowResolution.X, Graphics::WindowResolution.Y);

	for (unsigned int i = 0; i < Samples; ++i)
		glUniform3fv(glGetUniformLocation(AOShader->GetShaderID(), ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i].x);
	glUniformMatrix4fv(glGetUniformLocation(AOShader->GetShaderID(), "projection"), 1, false, &Graphics::MainCamera->GetProjection()[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return ssaoColorBuffer;
}

void SSAO::ResizeBuffer(unsigned int X, unsigned int Y)
{
	if (ssaoColorBuffer)
	{
		glDeleteTextures(1, &ssaoColorBuffer);
		glDeleteFramebuffers(1, &ssaoFBO);
	}
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Graphics::WindowResolution.X / ResolutionDivider, Graphics::WindowResolution.Y / ResolutionDivider, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

}
