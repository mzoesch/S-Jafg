// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Parameter.h"

#if !JAFG_WITH_REST_CLS
    #error "JAFG_WITH_REST_CLS is required to include this header."
#endif /* !JAFG_WITH_REST_CLS */

#include "Framework/Runnable.h"

namespace Jafg
{

namespace ReST
{

struct LRequest;
struct LResponse;

typedef TFunction<void(LRequest const&, LResponse*)> LCallback;

enum EStatusCode : i32 /* cpp-httplib */
{
    // Information responses
    Continue_100 = 100,
    SwitchingProtocol_101 = 101,
    Processing_102 = 102,
    EarlyHints_103 = 103,

    // Successful responses
    OK_200 = 200,
    Created_201 = 201,
    Accepted_202 = 202,
    NonAuthoritativeInformation_203 = 203,
    NoContent_204 = 204,
    ResetContent_205 = 205,
    PartialContent_206 = 206,
    MultiStatus_207 = 207,
    AlreadyReported_208 = 208,
    IMUsed_226 = 226,

    // Redirection messages
    MultipleChoices_300 = 300,
    MovedPermanently_301 = 301,
    Found_302 = 302,
    SeeOther_303 = 303,
    NotModified_304 = 304,
    UseProxy_305 = 305,
    unused_306 = 306,
    TemporaryRedirect_307 = 307,
    PermanentRedirect_308 = 308,

    // Client error responses
    BadRequest_400 = 400,
    Unauthorized_401 = 401,
    PaymentRequired_402 = 402,
    Forbidden_403 = 403,
    NotFound_404 = 404,
    MethodNotAllowed_405 = 405,
    NotAcceptable_406 = 406,
    ProxyAuthenticationRequired_407 = 407,
    RequestTimeout_408 = 408,
    Conflict_409 = 409,
    Gone_410 = 410,
    LengthRequired_411 = 411,
    PreconditionFailed_412 = 412,
    PayloadTooLarge_413 = 413,
    UriTooLong_414 = 414,
    UnsupportedMediaType_415 = 415,
    RangeNotSatisfiable_416 = 416,
    ExpectationFailed_417 = 417,
    ImATeapot_418 = 418,
    MisdirectedRequest_421 = 421,
    UnprocessableContent_422 = 422,
    Locked_423 = 423,
    FailedDependency_424 = 424,
    TooEarly_425 = 425,
    UpgradeRequired_426 = 426,
    PreconditionRequired_428 = 428,
    TooManyRequests_429 = 429,
    RequestHeaderFieldsTooLarge_431 = 431,
    UnavailableForLegalReasons_451 = 451,

    // Server error responses
    InternalServerError_500 = 500,
    NotImplemented_501 = 501,
    BadGateway_502 = 502,
    ServiceUnavailable_503 = 503,
    GatewayTimeout_504 = 504,
    HttpVersionNotSupported_505 = 505,
    VariantAlsoNegotiates_506 = 506,
    InsufficientStorage_507 = 507,
    LoopDetected_508 = 508,
    NotExtended_510 = 510,
    NetworkAuthenticationRequired_511 = 511,
};

struct LRequest final
{
    inline constexpr LRequest(void const* InPimpl) noexcept : Pimpl{InPimpl} { check(this->Pimpl) return; }

    ENGINE_API bool HasParameter(LString const& Key) const noexcept;
    ENGINE_API LString GetParameter(LString const& Key) const noexcept;

private:
    void const* Pimpl{};
};

struct LResponse final
{
    inline constexpr LResponse(void* InPimpl) noexcept : Pimpl{InPimpl} { check(this->Pimpl) return; }

    ENGINE_API void SetStatusCode(EStatusCode InStatusCode) noexcept;
    ENGINE_API void AddHeader(std::string&& InKey, std::string&& InValue) noexcept;
    ENGINE_API void SetContent(std::string&& Content, std::string&& ContentType) noexcept;

private:
    void* Pimpl{};
};

} /* ~Namespace ReST */

namespace Params
{

ENGINE_API extern LProgramParameter ReST_DisableAutoStart;
ENGINE_API extern LProgramParameter ReST_InstantStart;
ENGINE_API extern LProgramParameter ReST_Host;
ENGINE_API extern LProgramParameter ReST_Port;

} /* ~Namespace Params */

class ENGINE_API LReStCli final : public LRunnable
{
public:

    LReStCli() noexcept;
    PROHIBIT_REALLOC_OF_ANY_FORM(LReStCli)
    ~LReStCli() noexcept override = default;

    virtual ETaskExit::Type Initialize() override;
    virtual ETaskExit::Type Run() override;

    virtual void Exit() override;

    LReStCli& Get(std::string&& Pattern, ReST::LCallback&& Callback) noexcept;
    LReStCli& Get(std::string&& Pattern, LString&& Help, ReST::LCallback&& Callback) noexcept;

    bool IsServerRunning() const noexcept;

protected:

    virtual void OnStop(const ERunnableStopReason::Type InType) override;

    struct LReSTPattern
    {
        LString Pattern;
        LString Help;
        ReST::LCallback Callback;
    };

    void RegisterPattern(TArray<LReSTPattern>::iterator It);
    void InitializeDeferred();

    TArray<LReSTPattern> GetRegisteredPatterns;
};

} /* ~Namespace Jafg */
