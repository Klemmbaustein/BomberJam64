#pragma once
#include <Objects/WorldObject.h>
#include <GENERATED/GENERATED_Backpack.h>

class Backpack : public WorldObject
{
public:
	BACKPACK_GENERATED()
	void Begin() override;
	void Tick() override;
};