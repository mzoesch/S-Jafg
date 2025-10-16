// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

#if !JAFG_WITH_REST_CLS
    #error "JAFG_WITH_REST_CLS is required to include this header."
#endif /* !JAFG_WITH_REST_CLS */

#include "Async/Runnable.h"

namespace Jafg
{

namespace ReST
{

struct LRequest;
struct LResponse;

typedef TFunction<void(LRequest const&, LResponse*)> LCallback;

struct LRequest final
{
    inline LRequest(void const* InPimpl) noexcept : Pimpl{ InPimpl } { check( this->Pimpl ) return; }

private:
    void const* Pimpl{ nullptr };
};

struct LResponse final
{
    inline LResponse(void* InPimpl) noexcept : Pimpl{ InPimpl } { check( this->Pimpl ) return; }

    ENGINE_API void SetStatusCode(i32 InStatusCode) noexcept;
    ENGINE_API void AddHeader(std::string&& InKey, std::string&& InValue) noexcept;
    ENGINE_API void SetContent(std::string&& Content, std::string&& ContentType) noexcept;

private:
    void* Pimpl{ nullptr };
};

} /* ~Namespace ReST */

class LReStCli final : public LRunnable
{
public:

    ENGINE_API LReStCli() noexcept;
    PROHIBIT_REALLOC_OF_ANY_FORM(LReStCli)
    ENGINE_API ~LReStCli() noexcept override = default;

    virtual ETaskExit::Type Initialize() override;
    virtual ETaskExit::Type Run() override;

    virtual void Exit() override;

    ENGINE_API LReStCli& Get(std::string&& Pattern, ReST::LCallback&& Callback) noexcept;

protected:

    virtual void OnStop(const ERunnableStopReason::Type InType) override;
};

} /* ~Namespace Jafg */
