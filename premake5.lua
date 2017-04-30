-- premake5.lua
workspace "HelloWorld"
	location "Workspace" -- The location of generated files - .sln, etc.

	configurations { "Diagnostic", "Debug", "Release", "Profile", "Retail" }
	platforms { "Windows", "Linux" }

project "HelloWorld"
   kind "ConsoleApp"
   language "C++"
   targetdir "build/%{cfg.buildcfg}"

   files { "**.h", "**.c" }

   -- Platform Filters
   filter "platforms:*"
      architecture "x86_64"

   filter "platforms:Windows"
      defines "MOE_WINDOWS"

   filter "platforms:Linux"
      defines "MOE_LINUX"   


   -- Configuration filters
   filter "configurations:*"
	  flags { "ExtraWarnings", "C++11", "MultiProcessorCompile", "ShadowedVariables", "UndefinedIdentifiers" }
   
   filter "configurations:Diagnostic,Debug,Release"
      symbols "On"

   filter "configurations:Diagnostic"
      defines { "MOE_DIAGNOSTIC" }
	  optimize "Off"

   filter "configurations:Debug"
      defines { "MOE_DEBUG" }
	  optimize "Debug"
	  	 
   filter "configurations:Release,Profile"
      optimize "On"

   filter "configurations:Profile"
      defines { "MOE_PROFILE" }

   filter "configurations:Retail"
	  defines { "MOE_RETAIL" }
	  optimize "Full"
	  flags { "LinkTimeOptimization" }
	