#pragma once
#include <Objects/Objects.h>

class Snowman : public WorldObject
{
public:
	Snowman() : WorldObject(ObjectDescription("Snow man", 11)) {}
	void Begin() override;
};