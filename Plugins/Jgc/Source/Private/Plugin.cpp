// Copyright mzoesch. All rights reserved.

#include "Foreign/ForeignInclude.h"

DECLARE_INLINE_LOG_CATEGORY(LogJgcLifetime, Trace)

namespace Jgc
{

struct LJgcPluginLifetime final : public Jafg::LPluginLifetime
{
public:

    virtual void OnStartup() override;
    virtual void OnPrepareShutdown(const Jafg::EPluginShutdownReason::Type InReason) override;
    virtual void OnShutdown(const Jafg::EPluginShutdownReason::Type InReason) override;

    static void OnNativeStartup();
    static void OnNativeShutdown();
};

DEFINE_PLUGIN(JGC_API, LJgcPluginLifetime, JafgGameplayCore)

void LJgcPluginLifetime::OnStartup()
{
    LPluginLifetime::OnStartup();

    LOG_WARNING(LogJgcLifetime, "Called.")

    return;
}

void LJgcPluginLifetime::OnPrepareShutdown(const Jafg::EPluginShutdownReason::Type InReason)
{
    LPluginLifetime::OnPrepareShutdown(InReason);

    LOG_WARNING(LogJgcLifetime, "Called. Reason is [{}].", Jafg::LexToString(InReason))

    return;
}

void LJgcPluginLifetime::OnShutdown(const Jafg::EPluginShutdownReason::Type InReason)
{
    LPluginLifetime::OnShutdown(InReason);

    LOG_WARNING(LogJgcLifetime, "Called. Reason is [{}].", Jafg::LexToString(InReason))

    return;
}

void LJgcPluginLifetime::OnNativeStartup()
{
    LOG_WARNING(LogJgcLifetime, "Called.")

    return;
}

void LJgcPluginLifetime::OnNativeShutdown()
{
    LOG_WARNING(LogJgcLifetime, "Called.")

    return;
}

} /* ~Namespace Jgc */
