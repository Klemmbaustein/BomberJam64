#include <Objects/Components/Component.h>

void Component::Start()
{
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