#pragma once
#include <Objects/Components/Component.h>
#include <World/Graphics.h>
#include <Rendering/Camera/Camera.h>

class CameraComponent : public Component
{
public:
	virtual void Start(WorldObject* Parent) override;
	virtual void Tick() override;
	virtual void Destroy() override;

	void SetTransform(Transform NewTransform);
	Transform GetTransform();
	void SetFOV(float FOVinRadiants);
	CameraComponent();

	void Use();
protected:
	Camera ComponentCamera = Camera(2, Graphics::WindowResolution.X, Graphics::WindowResolution.Y, false);
	Transform RelativeTransform;
};