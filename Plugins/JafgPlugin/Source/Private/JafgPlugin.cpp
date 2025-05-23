// Copyright mzoesch. All rights reserved.

#include "Engine/Engine.h"
#include "Foreign/ForeignInclude.h"
#include "Core/JafgpNames.h"

DECLARE_INLINE_LOG_CATEGORY(LogJafgpLifetime, Trace)

namespace Jafgp
{

class LJafgPluginLifetime final : public Jafg::LPluginLifetime
{
    virtual void OnStartup() override;
};

DEFINE_PLUGIN(JAFGPLUGIN_API, LJafgPluginLifetime, JafgPlugin)

void LJafgPluginLifetime::OnStartup()
{
    if (GEngine)
    {
    }
    else
    {
        LOG_ERROR(LogJafgpLifetime, "Engine is invalid.")
    }

    return;
}

} /* ~Namespace Jafgp */
