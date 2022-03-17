#pragma once

// Chesster files
#include "Chesster_Unleashed/Core/Core.h"
#include "Chesster_Unleashed/Core/Logger.h"

// C++ standard library
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <array>
#include <memory>

// Microsoft
#ifdef CHESSTER_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <ws2tcpip.h>
#endif
