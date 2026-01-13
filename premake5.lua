workspace "MiniWorld"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["ImGui"] = "MiniWorld/vendor/imgui"
IncludeDir["glm"] = "MiniWorld/vendor/glm"

include "MiniWorld/vendor/imgui"

project "MiniWorld"
	location "MiniWorld"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "mwpch.h"
	pchsource "MiniWorld/src/mwpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links 
	{ 
		"ImGui",
		"d3d11.lib",
		"dxgi.lib",
		"d3dcompiler.lib",
		"dwmapi.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"MW_PLATFORM_WINDOWS",
			"MW_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "MW_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "MW_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "MW_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"MiniWorld/vendor/spdlog/include",
		"MiniWorld/src",
		"MiniWorld/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"MiniWorld"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"MW_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "MW_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "MW_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "MW_DIST"
		runtime "Release"
		optimize "on"
