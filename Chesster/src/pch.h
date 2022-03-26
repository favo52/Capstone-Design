#pragma once

// Chesster files
#include "Chesster/Core/Logger.h"

// C++ standard library
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <array>
#include <memory>

// Microsoft
#ifdef _WIN32
	#include <Windows.h>
	#include <ws2tcpip.h> // Winsock
#endif
