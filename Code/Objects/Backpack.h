#pragma once
#include <Objects/WorldObject.h>

class Backpack : public WorldObject
{
public:
	Backpack() : WorldObject(ObjectDescription("Backpack", 12)) {}
	void Begin() override;
	void Tick() override;
};