workspace "OpenAL Sandbox"
	startproject "OpenAL Sandbox"
	architecture "x64"

	staticruntime "On"

	configurations {
		"Debug",
		"Release",
	}

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"
		optimize "Off"

	filter "configurations:Release"
		defines "NDEBUG"
		symbols "Off"
		optimize "On"

	filter {}

	targetdir "bin/%{cfg.buildcfg}_%{cfg.architecture}"
	objdir "bin-int/%{cfg.buildcfg}_%{cfg.architecture}"

group "Vendor"
	include "vendor/openal-soft"
group ""

project "OpenAL Sandbox"
	language "C++"
	cppdialect "C++17"
	kind "ConsoleApp"

	files {
		"src/**.cpp",
		"include/**.h"
	}

	includedirs {
		"include",
		"vendor/openal-soft/include"
	}

	links {
		"openal-soft"
	}

	filter "system:windows"
		systemversion "latest"

	filter {}