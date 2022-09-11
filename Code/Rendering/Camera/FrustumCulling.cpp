#include "FrustumCulling.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/vec3.hpp>
namespace FrustumCulling
{
	Frustum createFrustumFromCamera(const Camera& cam)
	{
		float aspect = 16.f/9.f;
		float fovY = 2.f;
		float zNear = NearPlane;
		float zFar = FarPlane;
		Frustum frustum;
		const float halfVSide = zFar * tanf(fovY * .5f);
		const float halfHSide = halfVSide * aspect;
		const glm::vec3 frontMultFar = zFar * cam.lookAt;

		frustum.nearFace = { cam.Position + zNear * cam.lookAt, cam.lookAt };
		frustum.farFace = { cam.Position + frontMultFar, -cam.lookAt };
		frustum.rightFace = { cam.Position, glm::cross(cam.Up, frontMultFar + cam.Right * halfHSide) };
		frustum.leftFace = { cam.Position, glm::cross(frontMultFar - cam.Right * halfHSide, cam.Up) };
		frustum.topFace = { cam.Position, glm::cross(cam.Right, frontMultFar - cam.Up * halfVSide) };
		frustum.bottomFace = { cam.Position, glm::cross(frontMultFar + cam.Up * halfVSide, cam.Right) };
		return frustum;
	}
	Frustum CurrentCameraFrustum;
}