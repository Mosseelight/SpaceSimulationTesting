workspace "SpatialEngine"
    configurations { "Debug", "Release" }
    platforms { "Win64", "Unix" }
    defaultplatform("Unix")
    flags { "MultiProcessorCompile", "StaticRuntime" }

project "SpatialEngine"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    architecture "x86_64"
    targetdir "bin/%{cfg.buildcfg}"
    buildoptions { "-MMD -MP -Wall -c -g -m64 -march=native" }

    files { "src/**.hpp", "src/**.cpp", "src/**.h", "src/**.c" }
    includedirs { "src/include/**" }
    links { "SDL2", "SDL2_image" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter { "platforms:Unix" }
        toolset "clang"
        system "Linux"

    filter { "platforms:Win64" }
        system "Windows"