#include "HubTeleporter.h"
#include <Rendering/Mesh/JSModel.h>
#include <World/Assets.h>
#include <Objects/Components/PointLightComponent.h>

void HubTeleporter::Begin()
{
	TeleporterMesh = new MeshComponent();
	Attach(TeleporterMesh);
	TeleporterMesh->Load("Teleporter");

	auto CollMesh = JSM::LoadJSModel(Assets::GetAsset("Cube.jsm"));

	TeleportCollison = new CollisionComponent();
	Attach(TeleportCollison);
	TeleportCollison->Init(CollMesh.Vertices[0], CollMesh.Indices[0], Transform(Vector3(0, 0, 0), Vector3(), Vector3(1)));

	Properties.push_back(Object::Property("Target Scene", T_STRING, &TargetLevel));
	auto PointLight = new PointLightComponent();
	Attach(PointLight);
	PointLight->SetColor(Vector3(0, 0.5, 1));
	PointLight->SetFalloff(2);
	PointLight->SetIntensity(250);
	PointLight->SetRelativeLocation(Vector3(0, 0.2f, 0));
}

void HubTeleporter::Tick()
{
}

void HubTeleporter::Destroy()
{
}
