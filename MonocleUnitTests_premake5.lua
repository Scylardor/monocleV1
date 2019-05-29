-- MonocleUnitTests_premake5.lua
includeexternal "premake5.lua"

workspace "MonocleUnitTests"

	-- Initial config
	location "MonocleUnitTests" -- The directory of generated files - .sln, etc.
	configurations { "Diagnostic", "Debug", "Release", "Profile", "Shipping" }
	platforms { "Windows_Static", "Windows_DLL", "Linux_Static", "Linux_DLL" }
	targetdir "Monocle/Build/%{cfg.platform}/%{cfg.buildcfg}"
	objdir "MonocleUnitTests/Build/"
	language "C++"
	
	architecture "x86_64"

	-- At the moment use standard library for convenience.
	-- TODO: remove!
	defines { "MOE_STD_SUPPORT" }

	warnings "Extra"
	cppdialect "C++14"

	flags { "MultiProcessorCompile", "ShadowedVariables", "UndefinedIdentifiers" }

-- General filters (for all projects)
	-- Platform Filters
	filter "platforms:Windows*"
		-- _CRT_SECURE_NO_WARNINGS = don't warn when not using non-portable functions like fopen_s, etc.
		defines { "MOE_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
		system "windows"

	filter "platforms:Linux*"
		defines "MOE_LINUX"
		system "linux"

	filter "platforms:*DLL"
		defines { "MOE_USE_DLL" }

	-- Configuration filters
	filter "Diagnostic or Debug or Release"
		symbols "On"

	filter "Diagnostic"
		defines { "MOE_DIAGNOSTIC" }
		optimize "Off"

	filter "Debug"
		defines { "MOE_DEBUG" }
		optimize "Debug"

	filter "Release or Profile"
		optimize "On"

	filter "Profile"
		defines { "MOE_PROFILE" }

	filter "Shipping"
		defines { "MOE_SHIPPING" }
		optimize "Full"
		flags { "LinkTimeOptimization" }


-- Projects

project "UnitTests"
	kind "ConsoleApp"
	libdirs { "Monocle/Build/%{cfg.platform}/%{cfg.buildcfg}" }
	links { "Core", "GameFramework", "Input" }

	-- This seems to be mandatory on linux, you can't use the common objdir folder, or compile fails. Issue in investigation
	objdir "Monocle/Tests/Build/"
	
	files { "Monocle/Tests/UnitTests/*.cpp", "Monocle/ThirdParty/Catch/*" }
	includedirs { "Monocle/ThirdParty/Catch", "Monocle/" }

	-- we're using C++11
	defines "CATCH_CPP11_OR_GREATER"

	-- As all other projects of Monocle are libs, in DLL mode, this define needs to be removed
	removedefines { "MOE_DLL_EXPORT" }
	RemoveOtherPlatformSpecificFiles()

	-- The DLL version of the Linux executable doesn't seem to know where to get the DLL's on its own.
	-- TODO: check if that's still useful
	filter { "platforms:*DLL", "system:linux" }
		runpathdirs { "Monocle/Build/%{cfg.platform}/%{cfg.buildcfg}" }

