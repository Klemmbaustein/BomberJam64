#pragma once
#include <Math/Vector.h>
#include "glm/ext.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/matrix_decompose.hpp"

class Camera
{
public:

	Camera(float FOV, float Width, float Heigth, bool Ortho = false);
	void OnMouseMoved(float xRel, float yRel);
	void UpdateRotation();
	glm::mat4 getViewProj();

	void SetViewProjection(glm::mat4 in)
	{
		ViewProj = in;
	}

	void setProjection(glm::mat4 proj)
	{
		Projection = proj;
	}

	glm::mat4 getView()
	{
		return View;
	}

	void Update();

	void MoveForward(float amount)
	{
		Translate(glm::normalize(lookAt) * amount);
	}

	void MoveRight(float amount)
	{
		Translate(glm::normalize(glm::cross(lookAt, up)) * amount);
	}
	void MoveUp(float Ammount)
	{
		Translate(up * Ammount);
	}

	virtual void Translate(glm::vec3 V)
	{
		Position += V;
		View = glm::translate(View, V*-1.f);
	}

	glm::mat4 GetProjection()
	{
		return Projection;
	}

	void ReInit(float FOV, float Width, float Heigth, bool Ortho = false);

	Vector3 ForwardVectorFromScreenPosition(float x, float y);

	void SetView(glm::mat4 NewView)
	{
		View = NewView;
	}

	void SetRotation(Vector3 Rotation)
	{
		pitch = Rotation.X;
		yaw = Rotation.Y;
		roll = Rotation.Z;
	}

	float mouseSensitivity = 0.3f;
	glm::vec3 lookAt;
	Vector3 Rotation;
	float yaw;
	float pitch;
	float roll;
	glm::vec3 Position = glm::vec3(0.f);

protected:
	glm::vec3 up;

	float FOV;
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 ViewProj;
};