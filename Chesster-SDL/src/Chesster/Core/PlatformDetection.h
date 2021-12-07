// Platform detection using predefined macros

#ifdef _WIN32
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64 */
		#define CHESSTER_PLATFORM_WINDOWS
	#else
		/* Windows x86*/
		#error "x86 Builds are not supported!"
	#endif
/* __ANDROID__ needs to be checked before __linux__*/
#elif defined(__ANDROID__)
	#define CHESSTER_PLATFORM_ANDROID
#elif defined (__linux__)
	#define CHESSTER_PLATFORM_LINUX
	#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
	#error "Unknown platform!"
#endif
