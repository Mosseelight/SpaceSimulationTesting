workspace "SpatialEngine"
    configurations { "Debug", "Release" }
    platforms { "Win64", "Unix" }
    flags { "MultiProcessorCompile" }

project "SpatialEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    architecture "x86_64"
    targetdir "bin/%{cfg.buildcfg}"
    buildoptions { "-MMD -MP -Wall -c -g -m64 -O3 -march=native" }

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
        system "Linux"

    filter { "platforms:Win64" }
        system "Windows"