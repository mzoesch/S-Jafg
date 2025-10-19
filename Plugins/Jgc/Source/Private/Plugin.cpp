// Copyright mzoesch. All rights reserved.

#include "Foreign/ForeignInclude.h"
#include "JgcNames.h"

DECLARE_INLINE_LOG_CATEGORY(LogJgcLifetime, Trace)

namespace Jgc
{

struct LJgcPluginLifetime final : public Jafg::LPluginLifetime
{
public:

    virtual void OnStartup() override;
    virtual void OnPrepareShutdown(const Jafg::EPluginShutdownReason::Type InReason) override { }
    virtual void OnShutdown(const Jafg::EPluginShutdownReason::Type InReason) override { }

    static void OnNativeStartup() noexcept { }
    static void OnNativeShutdown() noexcept { }
};

DEFINE_PLUGIN(JGC_API, LJgcPluginLifetime, JafgGameplayCore)

void LJgcPluginLifetime::OnStartup()
{
    LPluginLifetime::OnStartup();
    LOG_VERBOSE(LogJgcLifetime, "Loading Jgc plugin.")

    check( GEngine )

#if WITH_LOCAL_LAYER
    LOG_VERBOSE(LogJgcLifetime, "Creating jgc levels.")
    if (GEngine->RegisterLevel
    (
        Jafg::LLevel
        {
            Name_LevelFrontend.ToString(),
            Jafg::EInputMode::UserInterface, true,
            Lal::LLinearColor::CadetBlue
        }
    ) == false)
    {
        LOG_WARNING(LogJgcLifetime, "Level [{}] is already registered.", Name_LevelFrontend.ToString())
    }
#endif /* WITH_LOCAL_LAYER */

#if WITH_LOCAL_LAYER
    LOG_VERBOSE(LogJgcLifetime, "Browsing to front-end level.")
    GEngine->Browse(GEngine->SummonWorld("JgcStartUp").Get(), Name_LevelFrontend.ToString(), {}, [](Jafg::LWorld& World)
    {
        LOG_VERBOSE(LogJgcLifetime, "Setting up local layer in front-end level.")

        auto& Frontend{ GEngine->GetLocalEgo().GetFrontend() };
        if (Frontend.GetSurfaceCount() != 1)
        {
            LOG_WARNING(LogJgcLifetime, "Expected exactly one surface at engine startup. Jgc does not support multiple surfaces in this stage if the application.")
        }
        else
        {
            auto& Surface{ Frontend.GetSurfaces()[0] };
            if (Surface.GetPossessed())
            {
                LOG_VERBOSE(LogJgcLifetime, "Local ego already possesses a persona controller. Skipping default jgc persona controller spawn.")
            }
            else
            {
                Jafg::APersonaController* Pc{ Jafg::SpawnActor<Jafg::APersonaController>(&World) };
                if (Surface.DoesPossess())
                {
                    /* Derived probably has a default spawn logic. */
                    check( Pc->IsSurfaceValid() )
                    LOG_VERBOSE(LogJgcLifetime, "Ego already possesses a controller. Skipping possess call.")
                }
                else
                {
                    Surface.Possess(Pc);
                }
            }
        }

        return;
    });
#endif /* WITH_LOCAL_LAYER */

    return;
}

} /* ~Namespace Jgc */
