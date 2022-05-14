project "Chesster"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "%{wks.location}/%{prj.name}/src/pch.cpp"

	files
	{
		"%{wks.location}/%{prj.name}/src/**.h",
		"%{wks.location}/%{prj.name}/src/**.cpp",
		"%{wks.location}/%{prj.name}/resource.h",
		"%{wks.location}/%{prj.name}/Chesster.rc",
		"%{wks.location}/%{prj.name}/Chesster.aps",
		"%{wks.location}/%{prj.name}/vendor/glm/glm/**.hpp",
		"%{wks.location}/%{prj.name}/vendor/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{wks.location}/%{prj.name}/src",
		"%{wks.location}/vendor/spdlog/include",
		"%{IncludeDir.SDL2}",
		"%{IncludeDir.SDL2_image}",
		"%{IncludeDir.SDL2_ttf}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}"
	}

	libdirs
	{
		"%{LibraryDir.SDL2_image}",
		"%{LibraryDir.SDL2_ttf}"
	}

	links
	{
		"SDL2.lib",
		"SDL2main.lib",
		"SDL2_image.lib",
		"SDL2_ttf.lib",
		"opengl32.lib",
		"ImGui",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		libdirs
		{
			"%{LibraryDir.SDL2_Debug}"
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		symbols "On"

		libdirs
		{
			"%{LibraryDir.SDL2_Release}"
		}

	filter "configurations:Dist"
		kind "WindowedApp"
		runtime "Release"
		optimize "On"
		symbols "Off"

		libdirs
		{
			"%{LibraryDir.SDL2_Release}"
		}

------------------------------------------------------------------------------------------
