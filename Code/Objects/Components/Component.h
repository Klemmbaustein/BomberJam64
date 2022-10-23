#pragma once
#include <Objects/WorldObject.h>

class ComponentSetter;

class Component
{
public:
	virtual void Start();
	virtual void Tick();
	virtual void Destroy();
	WorldObject* GetParent();
	friend class ComponentSetter;
protected:
	WorldObject* Parent = nullptr;
};

class ComponentSetter
{
public:
	static void SetParent(Component* c, WorldObject* p)
	{
		c->Parent = p;
	}
};