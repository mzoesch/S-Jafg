// Copyright mzoesch. All rights reserved.

#include "Foreign/ForeignInclude.h"
#include "Core/Parameter.h"
#include "Engine/Engine.h"
#include "Framework/Editor.h"

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

    /* No surface is ok. Maybe daemon or headless. */
    if (auto& Frontend{GEngine->GetLocalEgo().GetFrontend()}; Frontend.GetSurfaceCount() == 1)
    {
        auto const& Surface{Frontend.GetSurfaces()[0]};
        Jafg::ConstructWidget(Jafg::TNodeStaticInit<Jafg::WEditor>{Surface->GetViewport()});
    }
    else if (Frontend.GetSurfaceCount() > 1)
    {
        LOG_WARNING(LogJgcLifetime, "Expected exactly one surface at engine startup. Jgc does not support multiple surfaces in this stage of the application.")
    }
}

} /* ~Namespace <Anonymous> */

DEFINE_PLUGIN(JGC_API, LJgcPluginLifetime, JafgGameplayCore)
