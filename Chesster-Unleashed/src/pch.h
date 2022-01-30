#pragma once

// Chesster
#include "Chesster_Unleashed/Core/Core.h"
#include "Chesster_Unleashed/Core/Logger.h"
#include "Chesster_Unleashed/Core/Clock.h"

// General
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <chrono>

// Errors
#include <cassert>
#include <stdexcept>

// Data Structures
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

// C
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>

// Windows API
#ifdef CHESSTER_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <ws2tcpip.h>
#endif