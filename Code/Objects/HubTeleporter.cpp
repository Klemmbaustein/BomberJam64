#include "HubTeleporter.h"
#include <Rendering/Mesh/JSModel.h>

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
}

void HubTeleporter::Tick()
{
}

void HubTeleporter::Destroy()
{
}
