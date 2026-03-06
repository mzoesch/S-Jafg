// Copyright mzoesch. All rights reserved.

#include "Foreign/ForeignInclude.h"
#include "Runtime/Args.h"
#include "Fw/FwSupremePolicies.h"
#include "Engine/Engine.h"
#include "Framework/PersonaController.h"

DECLARE_INLINE_LOG_CATEGORY(LogJgcLifetime, Trace)

namespace
{

Jafg::LProgramParameter _JgcStartupLevel{
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

    check(GEngine)

    LOG_VERBOSE(LogJgcLifetime, "Creating jgc levels.")
#if WITH_LOCAL_LAYER
    if (GEngine->RegisterLevel
    (
        Jafg::LLevel
        {
            .Identifier = "LevelFrontend",
            .InputMode = Jafg::EInputModeBits::UserInterface | Jafg::EInputModeBits::InputSubsystem,
            .SupremePoliciesClass = Jgc::AFwSupremePolicies::StaticClass(),
        }
    ) == false)
    {
        LOG_WARNING(LogJgcLifetime, "Level [LevelFrontend] is already registered.")
    }
#endif /* WITH_LOCAL_LAYER */

    Jafg::Application::LProgramArgument const* StartupLevelArg{};
    if (Jafg::Application::HasCmdLineParameter(_JgcStartupLevel.Identifier, &StartupLevelArg))
    {
        jassert(StartupLevelArg->Value.has_value())
        LOG_VERBOSE(LogJgcLifetime, "Browsing to start-up level [{}] as specified on command line.", StartupLevelArg->Value.value())
    }

    LOG_VERBOSE(LogJgcLifetime, "Setting up local layer for frontend.")
    if (auto& Frontend{GEngine->GetLocalEgo().GetFrontend()}; Frontend.GetSurfaceCount() != 1)
    {
        LOG_WARNING(LogJgcLifetime, "Expected exactly one surface at engine startup. Jgc does not support multiple surfaces in this stage if the application.")
    }
    else
    {
        if (auto const& Surface{Frontend.GetSurfaces()[0]}; Surface->GetOwnedController())
        {
            LOG_VERBOSE(LogJgcLifetime, "Local ego already possesses a persona controller. Skipping default jgc frontend world creation and persona controller login.")
        }
        else
        {
            auto StartupWorld{GEngine->SummonWorld("JgcStartUp")};
            const LString TargetLevel{(StartupLevelArg ? StartupLevelArg->Value.value()
#if WITH_LOCAL_LAYER
                : "LevelFrontend"
#else /* WITH_LOCAL_LAYER */
                : Name_LevelListen.ToString()
#endif /* !WITH_LOCAL_LAYER */
                )};
            GEngine->Browse(StartupWorld, TargetLevel, {.OnWorldPostInit = [Surface = &*Surface](Jafg::LWorld& World){
                if (Surface->IsOwnedControllerValid())
                {
                    LOG_WARNING(LogJgcLifetime, "Surface [{}] already posses a persona controller through [{}@{}]. Skipping login",
                        Surface->GetHumanReadableName(),
                        Surface->GetOwnedControllerChecked()->GetWorld().GetHumanReadableName(),
                        Surface->GetOwnedControllerChecked()->GetWorld().GetUnderlyingLevelName()
                        )
                }
                else
                {
                    LOG_VERBOSE(LogJgcLifetime, "Logging in surface [{}] for [{}@{}].",
                        Surface->GetHumanReadableName(),
                        World.GetHumanReadableName(),
                        World.GetUnderlyingLevelName()
                        )
                    if (LString OutError; World.Login({Jafg::EIncomingConnectionRequest::Local, Surface}, &OutError) == nullptr)
                    {
                        LOG_FATAL(LogJgcLifetime, "Failed to login: {}", OutError)
                    }
                }
            }});
        }
    }

    return;
}

} /* ~Namespace <Anonymous> */

DEFINE_PLUGIN(JGC_API, LJgcPluginLifetime, JafgGameplayCore)
