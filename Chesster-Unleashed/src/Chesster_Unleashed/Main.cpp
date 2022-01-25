// Chesster's Entry-point

#include "pch.h"
#include "Chesster_Unleashed/Core/Application.h"

#include <SDL.h>

int main(int argc, char* args[])
{
	try
	{
		Chesster::Logger::Init();
		CHESSTER_INFO("Welcome to the Chesster UCI!");

		Chesster::Application* App = new Chesster::Application();
		App->Run();
		delete App;
	}
	catch (std::exception& e)
	{
		std::cerr << "EXCEPTION: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
