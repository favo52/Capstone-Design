#include "Application.h"

#include <stdexcept>
#include <iostream>

int main(int argc, char** argv)
try
{
    Chesster::Application* App = new Chesster::Application();
    App->Run();
    delete App;

    std::cout << "Enter any key to continue...";
    std::cin.get();
    return 0;
}
catch (const std::exception& e)
{
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
    return -1;
}
