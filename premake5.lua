-- premake5.lua
workspace "Monocle"

-- Declare functions HERE

function RemoveOtherPlatformSpecificFiles()
	filter "system:windows"
		removefiles "**/Linux/**"
	filter "system:linux"
		removefiles "**/Windows/**"
end

function SetBuildOptionsForLinuxDLL()
	filter { "platforms:*DLL", "system:linux" }
		buildoptions { "-fvisibility=hidden" }
end

-- End of functions sections


	-- Initial config
	location "Workspace" -- The directory of generated files - .sln, etc.
	configurations { "Diagnostic", "Debug", "Release", "Profile", "Shipping" }
	platforms { "Windows_Static", "Windows_DLL", "Linux_Static", "Linux_DLL" }
	targetdir "Build/%{cfg.platform}/%{cfg.buildcfg}"
	language "C++"


-- General filters (for all projects)
	-- Platform Filters

	filter "platforms:*"
		architecture "x86_64"

	filter "platforms:Windows*"
		-- _CRT_SECURE_NO_WARNINGS = don't warn when not using non-portable functions like fopen_s, etc.
		defines { "MOE_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
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
	configuration "*"
		defines { "MOE_STD_SUPPORT" } -- At the moment use standard library for convenience
		flags { "ExtraWarnings", "C++14", "MultiProcessorCompile", "ShadowedVariables", "UndefinedIdentifiers" }

	configuration { "Diagnostic", "Debug", "Release" }
		symbols "On"

	configuration "Diagnostic"
		defines { "MOE_DIAGNOSTIC" }
		optimize "Off"

	configuration "Debug"
		defines { "MOE_DEBUG" }
		optimize "Debug"

	configuration { "Release", "Profile" }
		optimize "On"

	configuration "Profile"
		defines { "MOE_PROFILE" }

	configuration "Shipping"
		defines { "MOE_SHIPPING" }
		optimize "Full"
		flags { "LinkTimeOptimization" }


-- Projects
project "MonocleCore"
	files { "MonocleSource/Core/**.cpp", "MonocleSource/Core/**.h", "MonocleSource/Core/**.hpp" }
	includedirs { "MonocleSource/Core/**/Include" }

	RemoveOtherPlatformSpecificFiles()
	SetBuildOptionsForLinuxDLL()


project "MonocleGraphics"
	files { "MonocleSource/Graphics/**.cpp", "MonocleSource/Graphics/**.h", "MonocleSource/Graphics/**.hpp" }
	-- Omit "MonocleSource/Core/**/Include" and it breaks compil; NEED TO REWORK INCLUDEDIRS POLICY FOR ALL PROJECTS
	includedirs { "MonocleSource/Graphics/**/Include", "MonocleSource/", "MonocleSource/Core/**/Include"}
	links { "MonocleCore", "opengl32" }

	-- TODO: I think we should be able to specify GL_MAJOR/GL_MINOR at compilation, and generate the appropriate glad files on the fly.
	-- We also need to manage platforms (e.g. do not include WGL but GLX on Linux...)
	filter "Diagnostic or Debug"
		files { "ExtLibs/glad45/debug/src/glad_wgl.c" }
		includedirs { "ExtLibs/glad45/debug/include"  }

	filter "Release or Profile or Shipping"
		files { "ExtLibs/glad45/release/src/glad_wgl.c" }
		includedirs { "ExtLibs/glad45/release/include"  }

	RemoveOtherPlatformSpecificFiles()
	SetBuildOptionsForLinuxDLL()

-- project "MonocleApplication"
	-- files { "MonocleSource/App/**.cpp", "MonocleSource/App/**.h", "MonocleSource/App/**.hpp" }
	-- includedirs { "MonocleSource/Core/**/Include" }
	-- links { "MonocleCore", "MonocleGraphics" }

	-- RemoveOtherPlatformSpecificFiles()
	-- SetBuildOptionsForLinuxDLL()
	
	
project "Test_Context"
	kind "ConsoleApp"
	files { "TestContext/*" }
	links { "MonocleCore", "MonocleGraphics" }
	includedirs { "MonocleSource/", "MonocleSource/Core/**/Include" }

	filter "Diagnostic or Debug"
		files { "ExtLibs/glad45/debug/src/glad_wgl.c" }
		includedirs { "ExtLibs/glad45/debug/include"  }

	filter "Release or Profile or Shipping"
		files { "ExtLibs/glad45/release/src/glad_wgl.c" }
		includedirs { "ExtLibs/glad45/release/include"  }

project "MonocleUnitTests"
	kind "ConsoleApp"
	links { "MonocleCore" }
	
	files { "Tests/UnitTests/*.cpp", "Tests/Catch/*" }
	includedirs { "Tests/Catch", "MonocleSource/Core/**/Include/" }
	
	defines "CATCH_CPP11_OR_GREATER"
	removedefines { "MOE_DLL_EXPORT" }

	filter { "platforms:*DLL", "system:linux" }
		runpathdirs { "Build/%{cfg.platform}/%{cfg.buildcfg}" }

