-- premake5.lua
workspace "Monocle"

	-- Initial config
	location "Workspace" -- The directory of generated files - .sln, etc.
	configurations { "Diagnostic", "Debug", "Release", "Profile", "Shipping" }
	platforms { "Windows Static", "Windows DLL", "Linux Static", "Linux DLL" }
	targetdir "Build/%{cfg.platform}/%{cfg.buildcfg}"
	language "C++"


-- General filters (for all projects)
	-- Platform Filters
	filter "platforms:*"
		architecture "x86_64"

	filter "platforms:Windows*"
		defines "MOE_WINDOWS"
		system "windows"

	filter "platforms:Linux*"
		defines "MOE_LINUX"
		system "linux"

	filter "platforms:*Static"
		kind "StaticLib"

	filter "platforms:*DLL"
		kind "SharedLib"

	filter "kind:SharedLib"
		defines { "MOE_USE_DLL", "MOE_DLL_EXPORT" }



	-- Configuration filters
	filter "configurations:*"
		defines { "MOE_STD_SUPPORT" } -- At the moment use standard library for convenience
		flags { "ExtraWarnings", "C++11", "MultiProcessorCompile", "ShadowedVariables", "UndefinedIdentifiers" }

	filter "configurations:Diagnostic (DLL),Debug (DLL),Release (DLL)"
		symbols "On"

	filter "configurations:Diagnostic (DLL)"
		defines { "MOE_DIAGNOSTIC" }
		optimize "Off"

	filter "configurations:Debug (DLL)"
		defines { "MOE_DEBUG" }
		optimize "Debug"

	filter "configurations:Release (DLL), Profile (DLL)"
		optimize "On"

	filter "configurations:Profile (DLL)"
		defines { "MOE_PROFILE" }

	filter "configurations:Shipping (Static)"
		defines { "MOE_SHIPPING" }
		optimize "Full"
		flags { "LinkTimeOptimization" }


-- Projects
project "MonocleCore"
	files { "MonocleSource/Core/**.cpp", "MonocleSource/Core/**.h", "MonocleSource/Core/**.hpp" }
	includedirs { "MonocleSource/Core/**/Include" }


project "MonocleUnitTests"
	kind "ConsoleApp"
	links { "MonocleCore" }
	
	files { "Tests/UnitTests/*.cpp", "Tests/Catch/*" }
	includedirs { "Tests/Catch", "MonocleSource/Core/**/Include/" }
	
	defines "CATCH_CPP11_OR_GREATER"
	removedefines { "MOE_DLL_EXPORT" }
