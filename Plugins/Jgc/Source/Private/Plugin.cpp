// Copyright mzoesch. All rights reserved.

#include "Foreign/ForeignInclude.h"
#include "Runtime/Args.h"
#include "Fw/FwSupremePolicies.h"
#include "Engine/Engine.h"
#include "Widgets/Editor.h"

DECLARE_INLINE_LOG_CATEGORY(LogJgcLifetime, Trace)

namespace
{

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
        Jafg::LLevel{
            .Identifier = "LevelFrontend",
            .InputMode = Jafg::EInputModeBits::HideMouseCursor,
            .SupremePoliciesClass = Jgc::AFwSupremePolicies::StaticClass(),
            }
    ) == false)
    {
        LOG_WARNING(LogJgcLifetime, "Level [LevelFrontend] is already registered.")
    }
#endif /* WITH_LOCAL_LAYER */

    if (auto& Frontend{GEngine->GetLocalEgo().GetFrontend()}; Frontend.GetSurfaceCount() != 1)
    {
        LOG_WARNING(LogJgcLifetime, "Expected exactly one surface at engine startup. Jgc does not support multiple surfaces in this stage of the application.")
    }
    else
    {
        auto const& Surface{Frontend.GetSurfaces()[0]};
        Jafg::ConstructWidget(Jafg::TWidgetStaticInit<Jafg::WEditor>{Surface->GetViewport()});
    }

    return;
}

} /* ~Namespace <Anonymous> */

DEFINE_PLUGIN(JGC_API, LJgcPluginLifetime, JafgGameplayCore)
