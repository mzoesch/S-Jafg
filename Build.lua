-- Copyright mzoesch. All rights reserved.

BinariesDir     = "Binaries/"
IntermediateDir = "Intermediate/"

OutputDir       = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}/%{prj.name}/"

targetdir ( BinariesDir .. OutputDir )
objdir ( IntermediateDir .. OutputDir )

language "C++"
cppdialect "C++20"

-- Important Vendor Header-onyl includes
includes = { }
-- Import Vendor includes
linkedIncludes = { }

workspace ( "Jafg" )
    architecture ( "x64" )
    startproject ( "Runtime" )
    configurations ( { "Debug", "Development", "Shipping", } )
    platforms ( { "Client", "Server" } )

    -- See https://premake.github.io/docs/Tokens/ for documentation
    -- prebuildcommands ( {
        --     "python ../Program.py run=BuildTask --prebuild \
        --         --%{wks.name} --%{wks.location} \
        --         --%{prj.name} --%{prj.location} --%{prj.language} --%{prj.group} \
        --         --%{cfg.longname} --%{cfg.shortname} --%{cfg.kind} --%{cfg.architecture} --%{cfg.platform} \
        --         --%{cfg.system} --%{cfg.buildcfg} --%{cfg.buildtarget} --%{cfg.linktarrget} --{cfg.objdir} \
        --         --%{file.path} --%{file.abspath} --%{file.relpath} --%{file.directory} --%{file.reldirectory} \
        --         --%{file.name} --%{file.basename} --%{file.extension} \
        --     "
    -- } )
    prebuildcommands ( { "python ../Program.py run=BuildTask --prebuild --%{cfg.buildcfg} --%{cfg.platform} --%{prj.name} --%{cfg.kind}", } )
    postbuildcommands ( { "python ../Program.py run=BuildTask --postbuild --%{cfg.buildcfg} --%{cfg.platform} --%{prj.name} --%{cfg.kind}", } )

    -- Incompatiple with /Yc - How do we fix this?
    -- flags ( { "MultiProcessorCompile" } )

-- -///////////////////////////////////////////////////////////////////////////
-- -// Hardware Platform filters
filter ( "system:windows" )
    systemversion ( "latest" )
    defines ( { "PLATFORM_WINDOWS", } )
    entrypoint "WinMainCRTStartup"
    linkoptions ( { "/SUBSYSTEM:WINDOWS", } )

    filter ( )
-- -// ~Hardware Platform filters
-- -///////////////////////////////////////////////////////////////////////////

-- -///////////////////////////////////////////////////////////////////////////
-- -// Config filters
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
-- -// ~Config filters
-- -///////////////////////////////////////////////////////////////////////////

-- -///////////////////////////////////////////////////////////////////////////
-- -// Platform filters
filter ( "platforms:Client" )
    defines ( "AS_CLIENT" )

filter ( "platforms:Server" )
    defines ( "AS_SERVER" )
filter ( )
-- -// ~Platform filters
-- -///////////////////////////////////////////////////////////////////////////

group ( "Runtime")
    project ( "Runtime" )
        location ( "Runtime" )
        kind ( "ConsoleApp" )

        includedirs ( { 
            "Runtime/Source/Public",
            "Core/Source/Public",
            "Core/Vendor/Deps/include",
        } )

        files ( { 
            "Runtime/Source/**.h",
            "Runtime/Source/**.hpp",
            "Runtime/Source/**.c",
            "Runtime/Source/**.cpp",
        } )

        links ( {
            "Static-Core",
            "Core/Vendor/Deps/lib/glfw3.lib",
        } )

        -- Somehow this does not work??
        -- The IDEA will just set the pch to /Yu but we, of course, need /Yc...
        -- pchheader ( "CoreAFX.h" )
        -- pchsource ( "Core/Source/Private/CoreAFX.cpp" )
group ( "" )

group ( "Static-Core")
    project ( "Static-Core" )
        location ( "Core" )
        kind ( "StaticLib" )

        includedirs ( { 
            "Core/Source/Public",
            "Core/Vendor/Deps/include",
        } )

        files ( { 
            "Core/Source/**.h",
            "Core/Source/**.hpp",
            "Core/Source/**.c",
            "Core/Source/**.cpp",
        } )

        links ( {
            "Core/Vendor/Deps/lib/glfw3.lib",
        } )

        -- Somehow this does not work??
        -- The IDEA will just set the pch to /Yu but we, of course, need /Yc...
        -- pchheader ( "CoreAFX.h" )
        -- pchsource ( "Core/Source/Private/CoreAFX.cpp" )
group ( "" )

group ("Programs")
    project ( "Router" )
        location ( "Programs" )
        kind ( "None" )
        files ( { 
            "Program.py",
            "GenerateSolutionFiles.bat",
            "Setup.bat",
            "Programs/Router.py",
        } )

    group ( "Programs/Tools")
        project ( "Shared" )
            location ( "Programs/Shared" )
            kind ( "None" )
            files ( { 
                "Programs/Shared/*.py",
            } )

        project ( "BuildTasks" )
            location ( "Programs/BuildTasks" )
            kind ( "None" )
            files ( { 
                "Programs/BuildTasks/*.py",
            } )

        project ( "BuildTool" )
            location ( "Programs/BuildTool" )
            kind ( "None" )
            files ( {
                "Programs/BuildTool/*.py", 
            } )
    group ( "Programs" )
group ( "" )
