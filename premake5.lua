workspace "Capstone-Design" -- Solution name
	architecture "x86_64"
	startproject "Chesster-Unleashed"

	configurations
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["SDL2"] = "%{wks.location}/vendor/SDL2/include"
IncludeDir["SDL2_image"] = "%{wks.location}/vendor/SDL2_image/include"
IncludeDir["SDL2_ttf"] = "%{wks.location}/vendor/SDL2_ttf/include"
IncludeDir["Glad"] = "%{wks.location}/vendor/Glad/include"
IncludeDir["imgui"] = "%{wks.location}/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/vendor/stb_image"
IncludeDir["Python"] = "%{wks.location}/vendor/Python/include"

LibraryDir = {}
LibraryDir["SDL2_Debug"] = "%{wks.location}/vendor/SDL2/lib/x64/Debug"
LibraryDir["SDL2_Release"] = "%{wks.location}/vendor/SDL2/lib/x64/Release"
LibraryDir["SDL2_image"] = "%{wks.location}/vendor/SDL2_image/lib/x64"
LibraryDir["SDL2_ttf"] = "%{wks.location}/vendor/SDL2_ttf/lib/x64"
LibraryDir["Python"] = "%{wks.location}/vendor/Python/libs"

group "Dependencies"
	include "vendor/Glad"
	include "vendor/imgui"
group ""

include "Chesster-SDL"
include "Chesster-Unleashed"
