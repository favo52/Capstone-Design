#pragma once

#include "PlatformDetection.h"

#ifdef CHESSTER_DEBUG
	#if defined(CHESSTER_PLATFORM_WINDOWS)
		#define CHESSTER_DEBUGBREAK() __debugbreak()
	#elif defined(CHESSTER_PLATFORM_LINUX)
		#include <signal.h>
		#define CHESSTER_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define CHESSTER_ENABLE_ASSERTS
#endif // CHESSTER_DEBUG

#ifdef CHESSTER_ENABLE_ASSERTS
	#define CHESSTER_ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); CHESSTER_DEBUGBREAK(); } }
#else
	#define CHESSTER_ASSERT(x, ...)
#endif // CHESSTER_ENABLE_ASSERTS
