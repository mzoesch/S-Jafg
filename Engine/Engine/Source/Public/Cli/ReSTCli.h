// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

#if !JAFG_WITH_REST_CLS
    #error "JAFG_WITH_REST_CLS is required to include this header."
#endif /* !JAFG_WITH_REST_CLS */

#include "Async/Runnable.h"

namespace Jafg
{

class LReStCli final : public LRunnable
{
public:

    ENGINE_API LReStCli() noexcept;

    PROHIBIT_REALLOC_OF_ANY_FORM(LReStCli)
    ENGINE_API ~LReStCli() noexcept override = default;

    virtual ETaskExit::Type Initialize() override;
    virtual ETaskExit::Type Run() override;
    virtual void Exit() override;

protected:

    virtual void OnStop(const ERunnableStopReason::Type InType) override;
};

} /* ~Namespace Jafg */
