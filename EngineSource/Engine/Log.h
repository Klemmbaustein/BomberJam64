#pragma once
#include <string>
#include <Math/Vector.h>
namespace Log
{
	void CreateNewLogMessage(std::string Text, Vector3 Color = Vector3(1, 1, 1));
	struct Message
	{
		Vector3 Color;
		std::string Text;
		int Ammount = 0;
		Message(std::string Text, Vector3 Color)
		{
			this->Color = Color;
			this->Text = Text;
		}
	};
	extern std::vector<Message> Messages;
}