// Chesster's Entry-point

/*
* Copyright 2021-present, Polytechnic University of Puerto Rico
* AUTHOR: Francisco A. Villagrasa
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

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
#include "Chesster/Core/Application.h"

#include <SDL.h>

int main(int argc, char* args[])
{
	try
	{
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
