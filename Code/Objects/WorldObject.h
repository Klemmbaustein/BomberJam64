#pragma once
#include <string>
#include "Math/Vector.h"
#include <TypeEnun.h>

class EngineContextMenu;

struct ObjectDescription
{
	ObjectDescription(std::string Name, uint32_t ID)
	{
		this->Name = Name;
		this->ID = ID;
	}
	std::string Name;
	uint32_t ID;
};

namespace Object
{
	struct Property
	{
		Property(std::string Name, TypeEnum Type, void* Data)
		{
			this->Name = Name;
			this->Type = Type;
			this->Data = Data;
		}
		std::string Name;
		TypeEnum Type;
		void* Data;
	};
}

class Component;

class WorldObject
{
public:
	WorldObject(ObjectDescription _d = ObjectDescription("Empty Object", 0))
	{
		TypeName = _d.Name;
		TypeID = _d.ID;
	}

	void Start(std::string ObjectName, Transform Transform);
	virtual void Destroy();
	virtual void Tick();
	virtual void Begin();
	void SetTransform(Transform NewTransform)
	{
		ObjectTransform = NewTransform;
	}

	Transform& GetTransform()
	{
		return ObjectTransform;
	}
	int Attach(Component* Component);
	void SetName(std::string Name)
	{
		this->Name = Name;
	}
	std::string GetName()
	{
		return Name;
	}

	ObjectDescription GetObjectDescription();

	void TickComponents();

	std::vector<Component*> GetComponents()
	{
		return Components;
	}

	virtual std::string Serialize();
	virtual void Detach(Component* C);
	virtual void Deserialize(std::string SerializedObject);
	virtual void OnPropertySet();
	std::string GetPropertiesAsString();
	void LoadProperties(std::string in);
	bool IsSelected = false;
	std::string CurrentScene;
protected:
	std::vector<Object::Property> Properties;
	std::string TypeName;
	uint32_t TypeID = 0;
	std::vector<Component*> Components;
	std::string Name = "Empty Object";
	friend EngineContextMenu;
	Transform ObjectTransform;
private:
};