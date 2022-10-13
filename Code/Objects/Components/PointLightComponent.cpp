#include "PointLightComponent.h"

void PointLightComponent::Start(WorldObject* Parent)
{
	this->Parent = Parent;
	Graphics::Lights.push_back(CurrentLight);
}

void PointLightComponent::Tick()
{
	if (CurrentLight != PreviousLight)
	{
		Update();
	}
}

void PointLightComponent::Destroy()
{
	Graphics::Lights.erase(Graphics::Lights.begin() + GetLightIndex());
}

void PointLightComponent::SetRelativeLocation(Vector3 NewLocation)
{
	CurrentLight.Position = NewLocation + Parent->GetTransform().Location;
}

Vector3 PointLightComponent::GetRelativeLocation()
{
	return CurrentLight.Position;
}

void PointLightComponent::SetColor(Vector3 NewColor)
{
	CurrentLight.Color = NewColor;
}

Vector3 PointLightComponent::GetColor()
{
	return CurrentLight.Color;
}

void PointLightComponent::SetIntensity(float NewIntensity)
{
	CurrentLight.Intensity = NewIntensity;
}

float PointLightComponent::GetIntensity()
{
	return CurrentLight.Intensity;
}

void PointLightComponent::SetFalloff(float NewFalloff)
{
	CurrentLight.Falloff = NewFalloff;
}

float PointLightComponent::GetFalloff()
{
	return CurrentLight.Falloff;
}

void PointLightComponent::Update()
{
	Graphics::Lights[GetLightIndex()] = CurrentLight;
	PreviousLight = CurrentLight;
}

size_t PointLightComponent::GetLightIndex()
{
	for (size_t i = 0; i < Graphics::Lights.size(); i++)
	{
		if (Graphics::Lights[i] == PreviousLight)
		{
			return i;
		}
	}
	throw("Could not find light index");
}
