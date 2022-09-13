#include "OrbGate.h"
#include <Save.h>
#include <Log.h>
std::vector<std::string> Textures =
{
	"Gates_X10",
	"Gates_X20",
	"Gates_X30"
};

std::vector<unsigned int> RequiredAmounts =
{
	10,
	20,
	30
};

void OrbGate::Begin()
{


	GateMeshComponent = new MeshComponent();
	Attach(GateMeshComponent);
	GateMeshComponent->Load("OrbGate");
	GateMeshComponent->SetUniform("u_diffuse", U_VEC3, Vector3(0.8, 0.8, 1).ToString(), 0);

	Properties.push_back(Object::Property("Gate Index", T_INT, &TextureIndex));
}

void OrbGate::OnPropertySet()
{
	GateMeshComponent->SetUniform("u_texture", U_TEXTURE, Textures[TextureIndex], 0);
	GateMeshComponent->SetUniform("u_usetexture", U_INT, std::to_string(1), 0);
	SaveGame MainSaveGame = SaveGame("Main");

	if (!IsInEditor)
	{
		if (!MainSaveGame.SaveGameIsNew())
		{
			if (std::stoi(MainSaveGame.GetPropterty("OrbsCollected").Value) > RequiredAmounts[TextureIndex])
			{
				Objects::DestroyObject(this);
				return;
			}
		}
	}
	GateCollisionComponent = new CollisionComponent();
	Attach(GateCollisionComponent);
	GateCollisionComponent->Init(GateMeshComponent->GetMeshData().Vertices, GateMeshComponent->GetMeshData().Indices);
}