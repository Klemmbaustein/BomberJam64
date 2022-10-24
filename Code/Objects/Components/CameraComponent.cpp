#pragma once
#include "CameraComponent.h"
#include <World/Stats.h>
#include <Rendering/Utility/Framebuffer.h>

void CameraComponent::Start()
{
	this->Parent = Parent;
}

void CameraComponent::Tick()
{
	Vector3 ParentRotation = Parent->GetTransform().Rotation.RadiantsToDegrees();
	ComponentCamera.Position = Parent->GetTransform().Location + RelativeTransform.Location;
	ComponentCamera.SetRotation(Vector3(ParentRotation.X + RelativeTransform.Rotation.X, ParentRotation.Y + RelativeTransform.Rotation.Y, ParentRotation.Z + RelativeTransform.Rotation.Z));
	ComponentCamera.Update();
}

void CameraComponent::Destroy()
{
}

void CameraComponent::SetTransform(Transform NewTransform)
{
	this->RelativeTransform = NewTransform;
}

Transform CameraComponent::GetTransform()
{
	return RelativeTransform;
}

void CameraComponent::SetFOV(float FOVinRadiants)
{
	ComponentCamera.ReInit(FOVinRadiants, Graphics::WindowResolution.X, Graphics::WindowResolution.Y, false);
}

CameraComponent::CameraComponent()
{
}

void CameraComponent::Use()
{
	if (!IsInEditor)
	{
		Graphics::MainFramebuffer->FramebufferCamera = &ComponentCamera;
		Graphics::MainCamera = &ComponentCamera;
	}
}