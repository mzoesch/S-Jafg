BinariesDir     = "Binaries/"
IntermediateDir = "Intermediate/"

OutputDir       = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}/%{prj.name}/"

targetdir ( BinariesDir .. OutputDir )
objdir ( IntermediateDir .. OutputDir )

language ( "C++" )
cppdialect ( "C++20" )

workspace ( "Jafg" )
    architecture ( "x64" )
    startproject ( "App" )
    configurations ( { "Debug", "Development", "Shipping" } )

filter ( "system:windows" )
    systemversion ( "latest" )
    defines ( { "PLATFORM_WINDOWS" } )

filter ( )

filter ( "configurations:Debug" )
    defines ( "IN_DEBUG" )
    runtime ( "Debug" )
    symbols ( "On" )
    optimize ( "Off" )

filter ( "configurations:Development" )
    defines ( "IN_DEVELOPMENT" )
    runtime ( "Release" )
    symbols ( "On" )
    optimize ( "On" )

filter ( "configurations:Shipping" )
    defines ( "IN_SHIPPING" )
    runtime ( "Release" )
    symbols ( "Off" )
    optimize ( "On" )

filter ( )

group ( "App" )
    project ( "App" )
        location ( "App" )
        kind ( "ConsoleApp" )

        includedirs ( { 
            "App/Source",
            "Core/Source",
        } )

        files ( { 
            "App/Source/**.h",
            "App/Source/**.cpp",
        } )

        links ( { "Core" } )

group ( "Core" )
    project ( "Core" )
        location ( "Core" )
        kind ( "StaticLib" )

        includedirs ( { 
            "Core/Source",
        } )

        files ( { 
            "Core/Source/**.h",
            "Core/Source/**.cpp",
        } )
