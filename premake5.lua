-- premake5.lua
workspace "Monocle"

-- Declare custom options HERE

-- Option to specify a graphics API to use at compile time.
-- On Linux, there's only OpenGL anyway.
-- But on Windows, you can have multiple flavors of D3D as well as OpenGL.
-- ATM, we link everything on Windows, even what we don't use.
-- The gfxapi flag then only defines what API gets used in the program.
-- Maybe we could be smarter by conditionally compiling/excluding unused gfx api's files.
-- TODO: improve that ?
newoption {
   trigger     = "gfxapi",
   value       = "API",
   description = "Choose a particular 3D API for rendering",
   allowed = {
      { "opengl4_5","OpenGL 4.5" },
      { "d3d11",  "Direct3D 11 (Windows only)" },
   }
}



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


-- ATM, only add as include dir the general OpenGL GLAD header's.
-- For include paths to keep working
function AddOpenGLFiles()
	filter "Debug or Diagnostic"
		includedirs { "ExtLibs/glad45/debug/"  }

	filter "Release or Profile or Shipping"
		includedirs { "ExtLibs/glad45/release/"  }
end

function AddWGLFiles()
	filter "Debug or Diagnostic"
		files { "ExtLibs/glad45/debug/glad/windows/glad_wgl.c" }
		includedirs { "ExtLibs/glad45/debug/glad/windows/"  }

	filter "Release or Profile or Shipping"
		files { "ExtLibs/glad45/release/glad/windows/glad_wgl.c" }
		includedirs { "ExtLibs/glad45/release/glad/windows/"  }
end

function AddGLXFiles()
	filter "Debug or Diagnostic"
		files { "ExtLibs/glad45/debug/linux/src/glad_glx.c" }
		includedirs { "ExtLibs/glad45/debug/linux/include"  }

	filter "Release or Profile or Shipping"
		files { "ExtLibs/glad45/release/linux/src/glad_glx.c" }
		includedirs { "ExtLibs/glad45/release/linux/include"  }
end

function AddGraphicsAPI()
	-- If no GFX api has been specified, use D3D on windows and openGL on Linux.
	if not _OPTIONS["gfxapi"] then
		filter "system:windows"
			_OPTIONS["gfxapi"] = "d3d11"
		filter "system:linux"
			_OPTIONS["gfxapi"] = "opengl4_5"
	end
	
	-- ATM, link everything in Windows, even if we're not using it... TODO: improve that ?
	-- The OpenGL lib name is "opengl32" even for 64-bit systems.
	AddOpenGLFiles()
	filter "system:windows"
		links { "opengl32", "d3d11", "dxgi" }
		AddWGLFiles()
	filter "system:linux"
		links { "opengl32" }
		AddGLXFiles()
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
	links { "MonocleCore" }

	-- TODO: I think we should be able to specify GL_MAJOR/GL_MINOR at compilation, and generate the appropriate glad files on the fly.
	-- We also need to manage platforms (e.g. do not include WGL but GLX on Linux...)
	AddGraphicsAPI()

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

	AddGraphicsAPI()
	RemoveOtherPlatformSpecificFiles()

project "MonocleUnitTests"
	kind "ConsoleApp"
	links { "MonocleCore" }
	
	files { "Tests/UnitTests/*.cpp", "Tests/Catch/*" }
	includedirs { "Tests/Catch", "MonocleSource/Core/**/Include/" }
	
	defines "CATCH_CPP11_OR_GREATER"
	removedefines { "MOE_DLL_EXPORT" }

	filter { "platforms:*DLL", "system:linux" }
		runpathdirs { "Build/%{cfg.platform}/%{cfg.buildcfg}" }

