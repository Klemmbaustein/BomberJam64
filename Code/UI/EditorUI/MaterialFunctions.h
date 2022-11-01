#pragma once
#include <string>

struct LoadError : std::exception
{
	LoadError(std::string In)
	{
		Error = ("Loading Error: " + In);
	}
	const char* what() const noexcept override
	{
		return Error.c_str();
	}
	std::string Error;
};

int MaterialUniformTypeStringToInt(std::string Type);

std::string MaterialUniformTypeToString(int Type);
