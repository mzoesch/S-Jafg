BinariesDir     = "Binaries/"
IntermediateDir = "Intermediate/"

OutputDir       = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}/%{prj.name}/"

targetdir ( BinariesDir .. OutputDir )
objdir ( IntermediateDir .. OutputDir )

language ( "C++" )
cppdialect ( "C++20" )

-- Important Vendor libraries
includeDir = { }
includeDir["GLFW"]   = "App/Vendor/GLFW/include"
includeDir["Glad"]   = "App/Vendor/Glad/include"
includeDir["spdlog"] = "Core/Vendor/spdlog/include"
includeDir["glm"]    = "Core/Vendor/glm/glm"

workspace ( "Jafg" )
    architecture ( "x64" )
    startproject ( "App" )
    configurations ( { "Debug", "Development", "Shipping", } )

    -- Incompatiple with /Yc - How do we fix this?
    -- flags ( { "MultiProcessorCompile" } )

filter ( "system:windows" )
    systemversion ( "latest" )
    defines ( { "PLATFORM_WINDOWS", } ) -- /* "GLFW_INCLUDE_NONE", */
    entrypoint "WinMainCRTStartup"
    linkoptions ( { "/SUBSYSTEM:WINDOWS", } )

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
            "App/Source/Public",
            "Core/Source/Public",
            "%{includeDir.GLFW}",
            "%{includeDir.Glad}",
            "%{includeDir.spdlog}",
            "%{includeDir.glm}",
        } )

        files ( { 
            "App/Source/**.h",
            "App/Source/**.cpp",
        } )

        links ( {
            "Core",
            "GLFW",
            "Glad",
            "opengl32.lib",
        } )

        -- Somehow this does not work??
        -- The IDEA will just set the pch to /Yu but we, of course, need /Yc...
        pchheader ( "CoreAFX.h" )
        pchsource ( "Core/Source/Private/CoreAFX.cpp" )
group ( "" )

group ( "Core" )
    project ( "Core" )
        location ( "Core" )
        kind ( "StaticLib" )

        includedirs ( { 
            "Core/Source",
            "Core/Source/Public",
            "%{includeDir.spdlog}",
        } )

        files ( { 
            "Core/Source/**.h",
            "Core/Source/**.cpp",
        } )

        -- Somehow this does not work??
        -- The IDEA will just set the pch to /Yu but we, of course, need /Yc...
        pchheader ( "CoreAFX.h" )
        pchsource ( "Core/Source/Private/CoreAFX.cpp" )
group ( "" )

group ( "Vendor/App" )
    include ( "App/Vendor/GLFW/Build.lua" )
    include ( "App/Vendor/Glad/Build.lua" )
group ( "" )

group ( "Vendor/Core" )
    include ( "Core/Vendor/Build.lua" )
group ( "" )
