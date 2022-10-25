#include "WorldObject.h"
#include "Components/Component.h"
#include <sstream>
#include <Objects/Objects.h>
#include <Objects/PlayerObject.h>

void WorldObject::Start(std::string ObjectName, Transform Transform)
{
	//Quick hack to make sure the player object is the first object to get destroyed, so we can save the rest of the world before it gets destroyed
	if (GetObjectDescription().ID == PlayerObject::GetID())
	{
		Objects::AllObjects.insert(Objects::AllObjects.begin(), this);
	}
	else
	{
		Objects::AllObjects.push_back(this);
	}
	Name = ObjectName;
	ObjectTransform = Transform;
	Begin();
}

void WorldObject::Destroy()
{
}

void WorldObject::Tick()
{
}

void WorldObject::Begin()
{
}

int WorldObject::Attach(Component* Component)
{
	Components.push_back(Component);
	ComponentSetter::SetParent(Component, this);
	Component->Start();
	return Components.size() - 1;
}


ObjectDescription WorldObject::GetObjectDescription()
{
	return ObjectDescription(TypeName, TypeID);
}

void WorldObject::TickComponents()
{
	for (int i = 0; i < Components.size(); i++)
	{
		Components.at(i)->Tick();
	}
}

std::string WorldObject::Serialize()
{
	return "";
}

void WorldObject::Detach(Component* C)
{
	for (int i = 0; i < Components.size(); i++)
	{
		if (Components[i] == C)
		{
			Components[i]->Destroy();
			delete Components[i];
			Components.erase(Components.begin() + i);
		}
	}
}

void WorldObject::Deserialize(std::string SerializedObject)
{
}

void WorldObject::OnPropertySet()
{
}

std::string WorldObject::GetPropertiesAsString()
{
	std::stringstream OutProperties;
	for (Object::Property p : Properties)
	{
		OutProperties << p.Name << ";" << p.Type << ";";
		switch (p.Type)
		{
		case T_FLOAT:
			OutProperties << std::to_string(*(float*)p.Data);
			break;
		case T_INT:
			OutProperties << std::to_string(*((int*)p.Data));
			break;
		case T_STRING:
			OutProperties << *((std::string*)p.Data);
			break;
		case T_VECTOR3:
			OutProperties << (*(Vector3*)p.Data).ToString();
			break;
		default:
			break;
		}
		OutProperties << "#";
	}
	return OutProperties.str();
}

void WorldObject::LoadProperties(std::string in)
{
	int i = 0;
	Object::Property CurrentProperty = Object::Property("", T_FLOAT, nullptr);
	std::string current;
	for (char c : in)
	{
		if (c != ';' && c != '#')
		{
			current = current + c;
		}
		else
		{

			switch (i)
			{
			case 0:
				CurrentProperty.Name = current;
				break;
			case 1:
				CurrentProperty.Type = (TypeEnum)std::stoi(current);
				break;
			case 2:
				for (Object::Property p : Properties)
				{
					if (p.Name == CurrentProperty.Name)
					{
						switch (CurrentProperty.Type)
						{
						case T_FLOAT:
							*((float*)p.Data) = std::stof(current);
							break;
						case T_INT:
							*((int*)p.Data) = std::stoi(current);
							break;
						case T_STRING:
							*((std::string*)p.Data) = (current);
							break;
						case T_VECTOR3:
							*((Vector3*)p.Data) = Vector3::stov(current);
							break;
						default:
							break;
						}
						break;
					}
				}
				i = -1;
				break;
			default:
				break;
			}
			current.clear();
			i++;
		}
	}
}
