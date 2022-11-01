#pragma once
#include <string>

/*
* For the compilation to work on both Linux and Windows, 
* This header implements functions that are different on Windows/Linux
*/

namespace OS
{
	void SetConsoleWindowVisible(bool Visible);

	std::string ShowOpenFileDialog();

	std::string GetOSString();

	void ClearConsoleWindow();

	void OpenFile(std::string Path);
}