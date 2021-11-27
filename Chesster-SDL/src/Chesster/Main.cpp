// Chesster's Entry-point

#include "pch.h"
#include "Application.h"

int main(int argc, char* args[])
try
{
	Chesster::Log::Init();
	CHESSTER_INFO("Welcome to the Chesster UCI!");

	Chesster::Application* App = new Chesster::Application();
	App->Run();
	delete App;

	return 0;
}
catch (std::exception& e)
{
	std::cerr << "EXCEPTION: " << e.what() << std::endl;
	return -1;
}
