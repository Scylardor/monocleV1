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

-------------------------
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
		includedirs { "Monocle/ThirdParty/glad45/debug/"  }

	filter "Release or Profile or Shipping"
		includedirs { "Monocle/ThirdParty/glad45/release/"  }
end

function AddWGLFiles()
	filter "Debug or Diagnostic"
		files { "Monocle/ThirdParty/glad45/debug/glad/windows/glad_wgl.c" }
		includedirs { "Monocle/ThirdParty/glad45/debug/glad/windows/"  }

	filter "Release or Profile or Shipping"
		files { "Monocle/ThirdParty/glad45/release/glad/windows/glad_wgl.c" }
		includedirs { "Monocle/ThirdParty/glad45/release/glad/windows/"  }
end

function AddGLXFiles()
	filter "Debug or Diagnostic"
		files { "Monocle/ThirdParty/glad45/debug/linux/src/glad_glx.c" }
		includedirs { "Monocle/ThirdParty/glad45/debug/linux/include"  }

	filter "Release or Profile or Shipping"
		files { "Monocle/ThirdParty/glad45/release/linux/src/glad_glx.c" }
		includedirs { "Monocle/ThirdParty/glad45/release/linux/include"  }
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
-----------------------------

	-- Initial config
	location "Monocle" -- The directory of generated files - .sln, etc.
	configurations { "Diagnostic", "Debug", "Release", "Profile", "Shipping" }
	platforms { "Windows_Static", "Windows_DLL", "Linux_Static", "Linux_DLL" }
	targetdir "Monocle/Build/%{cfg.platform}/%{cfg.buildcfg}"
	objdir "Monocle/Build/"
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
project "Core"
	location "Monocle/Core"
	files { "Monocle/Core/**.h", "Monocle/Core/*/Private/**.hpp", "Monocle/Core/*/Private/**.cpp" }
	includedirs { "Monocle/" }

	RemoveOtherPlatformSpecificFiles()
	SetBuildOptionsForLinuxDLL()


project "Graphics"
	location "Monocle/Graphics"
	files { "Monocle/Graphics/**.h", "Monocle/Graphics/*/Private/**.hpp", "Monocle/Graphics/*/Private/**.cpp" }
	includedirs { "Monocle/" }
	links { "Core" }

	-- TODO: I think we should be able to specify GL_MAJOR/GL_MINOR at compilation, and generate the appropriate glad files on the fly.
	-- We also need to manage platforms (e.g. do not include WGL but GLX on Linux...)
	AddGraphicsAPI()

	RemoveOtherPlatformSpecificFiles()
	SetBuildOptionsForLinuxDLL()

	project "UnitTests"
	location "Monocle/Tests"
	kind "ConsoleApp"
	links { "Core" }
	
	files { "Monocle/Tests/UnitTests/*.cpp", "Monocle/ThirdParty/Catch/*" }
	includedirs { "Monocle/ThirdParty/Catch", "Monocle/" }
	
	defines "CATCH_CPP11_OR_GREATER"
	removedefines { "MOE_DLL_EXPORT" }

	filter { "platforms:*DLL", "system:linux" }
		runpathdirs { "Build/%{cfg.platform}/%{cfg.buildcfg}" }

-- project "MonocleApplication"
	-- files { "Monocle/App/**.cpp", "Monocle/App/**.h", "Monocle/App/**.hpp" }
	-- includedirs { "Monocle/Core/**/Include" }
	-- links { "MonocleCore", "MonocleGraphics" }

	-- RemoveOtherPlatformSpecificFiles()
	-- SetBuildOptionsForLinuxDLL()


