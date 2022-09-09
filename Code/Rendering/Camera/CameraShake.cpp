#include "CameraShake.h"
#include <WorldParameters.h>

namespace CameraShake
{
	Vector3 CameraShakeTranslation;
}

float ShakeIntensity;

void CameraShake::PlayDefaultCameraShake(float Intensity)
{
	ShakeIntensity += Intensity;
}

void CameraShake::StopAllCameraShake()
{
	ShakeIntensity = 0;
}

void CameraShake::Tick()
{
	float ModifiedShakeIntensity = powf(ShakeIntensity, 2);
	CameraShakeTranslation = Vector3(sin(Time * 30) * ModifiedShakeIntensity, sin(Time * 25) * ModifiedShakeIntensity, 0);
	ShakeIntensity = std::max(ShakeIntensity - Performance::DeltaTime * 5, 0.f);
}

float CameraShake::GetCurrentCameraShakeIntensity()
{
	return ShakeIntensity;
}
