// Chesster's Entry-point

/*
* README
* 
* 
* 
* 
* 
* 
*/

#include "pch.h"
#include "Chesster_Unleashed/Core/Application.h"

#include <SDL.h>

int main(int argc, char* args[])
{
	try
	{
		// Instantiate the application, run it, and delete everything when closed
		Chesster::Application* App = new Chesster::Application("CHESSTER!");
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
