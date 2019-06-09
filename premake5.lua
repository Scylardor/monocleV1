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
	filter { "kind:SharedLib", "system:linux" }
		buildoptions { "-fvisibility=hidden" }

end


-- ATM, only add as include dir the general OpenGL GLAD header's.
-- For include paths to keep working
function IncludeOpenGLFiles()
	filter "Debug or Diagnostic"
		includedirs { "Monocle/ThirdParty/glad45/debug/"  }

	filter "Release or Profile or Shipping"
		includedirs { "Monocle/ThirdParty/glad45/release/"  }
end

function AddWGLFiles()
	-- Make additional includedirs because GLAD expects its own folder to be in the include path, as it does: #include <glad_wgl.h>
	filter "Debug or Diagnostic"
		files { "Monocle/ThirdParty/glad45/debug/glad/windows/glad_wgl.c" }
		includedirs { "Monocle/ThirdParty/glad45/debug/glad/windows/"  }

	filter "Release or Profile or Shipping"
		files { "Monocle/ThirdParty/glad45/release/glad/windows/glad_wgl.c" }
		includedirs { "Monocle/ThirdParty/glad45/release/glad/windows/"  }
end

function AddGLXFiles()
	-- Make additional includedirs because GLAD expects its own folder to be in the include path, as it does: #include <glad_glx.h>
	filter "Debug or Diagnostic"
		files { "Monocle/ThirdParty/glad45/debug/glad/linux/src/glad_glx.c" }
		includedirs { "Monocle/ThirdParty/glad45/debug/glad/linux/include"  }

	filter "Release or Profile or Shipping"
		files { "Monocle/ThirdParty/glad45/release/glad/linux/src/glad_glx.c" }
		includedirs { "Monocle/ThirdParty/glad45/release/glad/linux/include"  }
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
	IncludeOpenGLFiles()
	
	filter "system:windows"
		links { "opengl32", "d3d11", "dxgi" }
		AddWGLFiles()
		
	filter "system:linux"
		links { "GL", "dl", "X11" } -- lib GL is dynamically loaded at runtime so we need dl for dlopen etc.
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

	filter "platforms:*Static"
		kind "StaticLib"

	filter "platforms:*DLL"
		kind "SharedLib"
		defines { "MOE_USE_DLL", "MOE_DLL_EXPORT" }

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
	AddGraphicsAPI()

	RemoveOtherPlatformSpecificFiles()
	SetBuildOptionsForLinuxDLL()

	
project "Input"
	location "Monocle/Input"
	files { "Monocle/Input/**.h", "Monocle/Input/Private/**.hpp", "Monocle/Input/Private/**.cpp" }
	includedirs { "Monocle/" }
	links { "Core" }

	RemoveOtherPlatformSpecificFiles()
	SetBuildOptionsForLinuxDLL()

	
project "Windowing"
	location "Monocle/Windowing"
	files { "Monocle/Windowing/**.h", "Monocle/Windowing/Private/**.hpp", "Monocle/Windowing/Private/**.cpp" }
	includedirs { "Monocle/" }
	links { "Core", "Graphics", "Input" }

	AddGraphicsAPI()
	RemoveOtherPlatformSpecificFiles()
	SetBuildOptionsForLinuxDLL()

	
project "GameFramework"
	location "Monocle/GameFramework"
	files { "Monocle/GameFramework/**.h", "Monocle/GameFramework/Private/**.hpp", "Monocle/GameFramework/**/Private/**.cpp" }
	includedirs { "Monocle/", "Monocle/GameFramework" }
	links { "Core" }

	RemoveOtherPlatformSpecificFiles()
	SetBuildOptionsForLinuxDLL()
	

-- Uncomment this if you want to include the Unit Tests project in the main engine solution
-- project "UnitTests"
	-- location "Monocle/Tests"
	-- kind "ConsoleApp"
	-- links { "Core" }

	-- -- This seems to be mandatory on linux, you can't use the common objdir folder, or compile fails. Issue in investigation
	-- objdir "Monocle/Tests/Build/"
	
	-- files { "Monocle/Tests/UnitTests/*.cpp", "Monocle/ThirdParty/Catch/*" }
	-- includedirs { "Monocle/ThirdParty/Catch", "Monocle/" }

	-- -- we're using C++11
	-- defines "CATCH_CPP11_OR_GREATER"

	-- -- As all other projects of Monocle are libs, in DLL mode, this define needs to be removed
	-- removedefines { "MOE_DLL_EXPORT" }
	-- RemoveOtherPlatformSpecificFiles()

	-- -- The DLL version of the Linux executable doesn't seem to know where to get the DLL's on its own.
	-- -- TODO: check if that's still useful
	-- filter { "platforms:*DLL", "system:linux" }
		-- runpathdirs { "Monocle/Build/%{cfg.platform}/%{cfg.buildcfg}" }

project "MonocleTestApp"
	location "MonocleTestApp"
	kind "ConsoleApp"
	files { "MonocleTestApp/*.cpp", "MonocleTestApp/*.h", "MonocleTestApp/*.hpp" }
	includedirs { "Monocle" }
	links { "Core", "Graphics", "Windowing", "GameFramework", "Input" }
	
	-- As all other projects of Monocle are libs, in DLL mode, this define needs to be removed
	removedefines { "MOE_DLL_EXPORT" }

	AddGraphicsAPI()
	RemoveOtherPlatformSpecificFiles()