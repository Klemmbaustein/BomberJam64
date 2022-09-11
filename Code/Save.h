#pragma once
#include <string>
#include <map>
#include <TypeEnun.h>

struct SaveGame
{
	SaveGame(std::string SaveName);

	struct SaveProperty
	{
		SaveProperty(std::string Name, std::string Value, TypeEnum Type);
		SaveProperty() {}
		std::string Name = "undefined"; std::string Value = "null"; TypeEnum Type = T_NULL;
		auto operator<=>(SaveProperty const&) const = default;
	};

	SaveProperty GetPropterty(std::string Name);

	void SetPropterty(SaveProperty S);
	void ClearProperties()
	{
		Properties.clear();
	}
	~SaveGame();

private:
	std::map<std::string, SaveProperty> Properties;
	std::string OpenedSave;
};