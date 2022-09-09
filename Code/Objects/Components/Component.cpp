#include <Objects/Components/Component.h>

void Component::Start(WorldObject* Parent)
{
	this->Parent = Parent;
}

void Component::Tick()
{
}

void Component::Destroy()
{
}


WorldObject* Component::GetParent()
{
	return Parent;
}