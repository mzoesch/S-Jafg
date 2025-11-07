// Copyright mzoesch. All rights reserved.

#include "Foreign/ForeignInclude.h"
#include "Runtime/Args.h"
#include "Core/JgcNames.h"
#include "Svw/SvwSupremePolicies.h"

DECLARE_INLINE_LOG_CATEGORY(LogJgcLifetime, Trace)

namespace
{

Lal::LProgramParameter _JgcStartupLevel{
    "Jgc.StartupLevel",
    "Specifies the level to start Jgc with.",
    };

class LJgcPluginLifetime final : public Jafg::LPluginLifetime
{
public:

    virtual void OnFinishedLoading() override;
};

void LJgcPluginLifetime::OnFinishedLoading()
{
    LPluginLifetime::OnFinishedLoading();
    LOG_VERBOSE(LogJgcLifetime, "Loading Jgc plugin.")

    check( GEngine )

    LOG_VERBOSE(LogJgcLifetime, "Creating jgc levels.")
#if WITH_LOCAL_LAYER
   if (GEngine->RegisterLevel
   (
       Jafg::LLevel
       {
           Name_LevelFrontend.ToString(),
           Jafg::EInputMode::Both, true,
           Lal::LLinearColor::CadetBlue
       }
   ) == false)
   {
       LOG_WARNING(LogJgcLifetime, "Level [{}] is already registered.", Name_LevelFrontend.ToString())
   }
#endif /* WITH_LOCAL_LAYER */
    if (GEngine->RegisterLevel
    (
        Jafg::LLevel
        {
            Name_LevelSvw.ToString(),
            Jafg::EInputMode::InputSubSystem, false,
            Lal::LLinearColor::Gray,
            true, false, TArray<Jafg::LLevelSkyboxMap>
            {
                Jafg::LLevelSkyboxMap
                {
                    "Night",
                    TArray<Jafg::LEnginePath>
                    {
                        Jafg::LEnginePath{ Jafg::EEnginePaths::Textures, "Misc/SbNight.png" },
                    },
                },
            },
            Jgc::JSvwSupremePolicies::StaticClass()
        }
    ) == false)
    {
        LOG_WARNING(LogJgcLifetime, "Level [{}] is already registered.", Name_LevelSvw.ToString())
    }

    Jafg::Application::LProgramArgument const* StartupLevelArg{ nullptr };
    if (Jafg::Application::HasCmdLineParameter(_JgcStartupLevel.Identifier, &StartupLevelArg))
    {
        jassert( StartupLevelArg->Value.has_value() )
        LOG_VERBOSE(LogJgcLifetime, "Browsing to start-up level [{}] as specified on command line.", StartupLevelArg->Value.value())
    }

    LOG_VERBOSE(LogJgcLifetime, "Browsing to startup level.")
    GEngine->Browse(GEngine->SummonWorld("JgcStartUp").Get()
        , (StartupLevelArg ? StartupLevelArg->Value.value()
#if WITH_LOCAL_LAYER
            : Name_LevelFrontend.ToString()
#else /* WITH_LOCAL_LAYER */
            : Name_LevelListen.ToString()
#endif /* !WITH_LOCAL_LAYER */
          )
        , [](Jafg::LWorld& World)
    {
#if WITH_LOCAL_LAYER
        LOG_VERBOSE(LogJgcLifetime, "Setting up local layer in front-end level.")
        auto& Frontend{ GEngine->GetLocalEgo().GetFrontend() };
        if (Frontend.GetSurfaceCount() != 1)
        {
            LOG_WARNING(LogJgcLifetime, "Expected exactly one surface at engine startup. Jgc does not support multiple surfaces in this stage if the application.")
        }
        else
        {
            auto& Surface{ Frontend.GetSurfaces()[0] };
            if (Surface->GetController())
            {
                LOG_VERBOSE(LogJgcLifetime, "Local ego already possesses a persona controller. Skipping default jgc persona controller spawn.")
            }
            else
            {
                auto* Pc{ World.Login(Jafg::LTransientPersona{ Jafg::EIncomingConnectionRequest::Local, Surface.get() }) };
                check( Pc )
                check( Pc->HasBegunLife() == false )
            }
        }
#endif /* WITH_LOCAL_LAYER */

        return;
    });

    return;
}

} /* ~Namespace <Anonymous> */

DEFINE_PLUGIN(JGC_API, LJgcPluginLifetime, JafgGameplayCore)
