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
#include <thread>
#include <chrono>

// Microsoft
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <ws2tcpip.h> // Winsock
#endif
