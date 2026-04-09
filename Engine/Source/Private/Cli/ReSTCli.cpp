// Copyright mzoesch. All rights reserved.

#include "Core/Application.h"

namespace Jafg::Params
{
ENGINE_API LProgramParameter ReST_DisableAutoStart{{
    .Identifier = "Jafg.ReSTCli.DisableAutoStart",
    .Description = "Whether to disable automatic starting of the ReST CLI server on engine start "
                   "(only effective if Jafg::JReSTCliPreferences::bAutoStart is true).",
    }};

ENGINE_API LProgramParameter ReST_InstantStart{{
    .Identifier = "Jafg.ReSTCli.InstantStart",
    .Description = "Whether to start the ReST CLI server on engine start "
                   "(only effective if Jafg::JReSTCliPreferences::bAlwaysDisable is false).",
    }};

ENGINE_API LProgramParameter ReST_Host{{
    .Identifier = "Jafg.ReSTCli.Host",
    .Description = "Override the default host to bind the ReST CLI server to (default is Jafg::JReSTCliPreferences::Host).",
    .Flags = EProgramParameterBits::Value,
    }};

ENGINE_API LProgramParameter ReST_Port{{
    .Identifier = "Jafg.ReSTCli.Port",
    .Description = "Override the default port to start the ReST CLI server on (default is Jafg::JReSTCliPreferences::Port).",
    .Flags = EProgramParameterBits::Value,
    }};

} /* ~Namespace Jafg::Params */

#if JAFG_WITH_REST_CLS

#include "Serialization/Json.h"
#include "Cli/ReSTCli.h"
#include "Async/TaskUtility.h"
#include "Cli/ReSTCliPreferences.h"

#define CPPHTTPLIB_THREAD_POOL_COUNT                                    1
#define CPPHTTPLIB_NO_EXCEPTIONS                                        1
#include "httplib.h"

namespace
{

//# There can only ever be one server instance.
TUnique<httplib::Server> Server;

} /* ~Namespace <Anonymous> */

bool Jafg::ReST::LRequest::HasParameter(LString const& Key) const noexcept
{
    return static_cast<httplib::Request const*>(this->Pimpl)->has_param(Key);
}

LString Jafg::ReST::LRequest::GetParameter(LString const& Key) const noexcept
{
    return static_cast<httplib::Request const*>(this->Pimpl)->get_param_value(Key);
}

void Jafg::ReST::LResponse::SetStatusCode(EStatusCode InStatusCode) noexcept
{
    static_cast<httplib::Response*>(this->Pimpl)->status = static_cast<i32>(InStatusCode);
}

void Jafg::ReST::LResponse::AddHeader(std::string&& InHeader, std::string&& InValue) noexcept
{
    static_cast<httplib::Response*>(this->Pimpl)->set_header(std::move(InHeader), std::move(InValue));
}

void Jafg::ReST::LResponse::SetContent(std::string&& Content, std::string&& ContentType) noexcept
{
    static_cast<httplib::Response*>(this->Pimpl)->set_content(std::move(Content), std::move(ContentType));
}

Jafg::LReStCli::LReStCli() noexcept : LRunnable("ReSTCli")
{
    this->Get("/help", [this](ReST::LRequest const& Req, ReST::LResponse* Res)
    {
        json Out;
        auto& Get = Out["Get"];
        for (auto const& Pattern : this->GetRegisteredPatterns)
        {
            json Entry;
            Entry["pattern"] = Pattern.Pattern;
            Entry["help"] = Pattern.Help;
            Get.emplace_back(std::move(Entry));
            continue;
        }

        Res->SetContent(Out.dump(), "application/json");
        return;
    });

    return;
}

Jafg::ETaskExit::Type Jafg::LReStCli::Initialize()
{
    if (ETaskExit::Type SuperRet{LRunnable::Initialize()}; SuperRet != ETaskExit::Success)
    {
        return SuperRet;
    }

    LOG_VERBOSE(LogReST, "Initializing ReSTCli.")

    auto& Prefs{GetSingleton<JReSTCliPreferences>()};

    check( Prefs.bAlwaysDisable == false )

    ::Server = std::make_unique<httplib::Server>();
    check( ::Server.get() )

    if constexpr (IS_COMPILED_LOG(LogReST, Verbose))
    {
        ::Server->set_logger([](httplib::Request const& Req, httplib::Response const& Res)
        {
            std::stringstream ss;

            ss << Req.method << " " << Req.path;
            if (Req.params.size() > 0)
            {
                ss << "{";
                bool bFirst{ true };
                for (const auto& Param : Req.params)
                {
                    if (bFirst == false)
                    {
                        ss << ",";
                    }
                    bFirst = false;
                    ss << Param.first << ":" << Param.second;
                }
                ss << "}";
            }
            ss << " -> " << Res.status;
            ss << " remote[" << Req.remote_addr << ":" << Req.remote_port << "]";
            if (Req.method == "POST" || Req.method == "PUT")
            {
                ss << " (" << Req.body.size() << " bytes)";
            }

            LOG_VERBOSE(LogReST, "{}", ss.str())

            return;
        });
    }

    if constexpr (IS_COMPILED_LOG(LogReST, Trace))
    {
        ::Server->set_pre_compression_logger([](httplib::Request const& Req, httplib::Response const& Res)
        {
            std::stringstream ss;

            ss << "Pre-compressed response for " << Req.method << " " << Req.path << " -> " << Res.status;
            if (Req.method == "POST" || Req.method == "PUT")
            {
                ss << " (" << Req.body.size() << " bytes)";
            }

            LOG_TRACE(LogReST, "{}", ss.str())

            return;
        });
    }

    if constexpr (IS_COMPILED_LOG(LogReST, Error))
    {
        ::Server->set_error_logger([](httplib::Error const& Err, httplib::Request const* Req)
        {
            std::stringstream ss;
            ss << httplib::to_string(Err) << " while processing request";
            if (Req)
            {
                ss << ", client [" << Req->get_header_value("X-Forwarded-For")
                   << ", request [" << Req->method << " " << Req->path << " " << Req->version << "]"
                   << ", host [" << Req->get_header_value("Host") << "]";
            }

            LOG_ERROR(LogReST, "{}", ss.str())

            return;
        });
    }

    ::Server->Options(".*", [](httplib::Request const& Req, httplib::Response& Res)
    {
        Res.status = httplib::NoContent_204;
        return;
    });

    ::Server->set_default_headers(
    {
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    ::Server->Get("/docs", [](httplib::Request const& Req, httplib::Response& Res)
    {
        check(::Server.get())
        // TODO: Fork the repo and add visitors...
        Res.set_content("{}", "application/json");
        return;
    });

    return ETaskExit::Success;
}

Jafg::ETaskExit::Type Jafg::LReStCli::Run()
{
    check(Tasks::IsOnReSTCliThread())

    if (::Server.get() == nullptr)
    {
        LOG_WARNING(LogReST, "Started ReSTCli thread without its server being initialized.")
        return ETaskExit::Success;
    }
    check(::Server->is_running() == false)

    LOG_VERBOSE(LogReST, "Starting ReSTCli.")
    auto& Prefs{GetSingleton<JReSTCliPreferences>()};

    LString Host{*Prefs.Host};
    if (auto* Arg{Application::GetCommandLineArgument(Params::ReST_Host)})
    {
        Host = Arg->GetValue();
    }
    i32 Port{*Prefs.Port};
    if (auto* Arg{Application::GetCommandLineArgument(Params::ReST_Port)})
    {
        Serde::FromString(&Port, Arg->GetValue());
    }
    LOG_VERBOSE(LogReST, "Listening on [{}:{}].", Host, Port)

    this->InitializeDeferred();

    ::Server->set_keep_alive_max_count(*Prefs.KeepAliveMaxRequests);
    ::Server->set_keep_alive_timeout(static_cast<time_t>(*Prefs.KeepAliveTimeoutInSeconds));
    ::Server->set_read_timeout(
        static_cast<time_t>(*Prefs.ReadTimeoutInSeconds),
        static_cast<time_t>((*Prefs.ReadTimeoutInSeconds - static_cast<time_t>(*Prefs.ReadTimeoutInSeconds)) * maths::s2mus_d)
        );
    ::Server->set_write_timeout(
        static_cast<time_t>(*Prefs.WriteTimeoutInSeconds),
        static_cast<time_t>((*Prefs.WriteTimeoutInSeconds - static_cast<time_t>(*Prefs.WriteTimeoutInSeconds)) * maths::s2mus_d)
        );
    ::Server->set_idle_interval(
        static_cast<time_t>(*Prefs.IdleIntervalInSeconds),
        static_cast<time_t>((*Prefs.IdleIntervalInSeconds - static_cast<time_t>(*Prefs.IdleIntervalInSeconds)) * maths::s2mus_d)
        );

    ::Server->set_payload_max_length(static_cast<size_t>(Prefs.PayLoadMaxLength));

    ::Server->set_tcp_nodelay(*Prefs.TcpNoDelay);
    ::Server->set_ipv6_v6only(*Prefs.Ipv6_v6Only);

    ::Server->listen(Host, 8080);

    check( ::Server->is_running() == false )
    LOG_VERBOSE(LogReST, "ReSTCli stopped.")
    return ETaskExit::Success;
}

void Jafg::LReStCli::Exit()
{
    check(Tasks::IsOnReSTCliThread())

    LRunnable::Exit();

    if (::Server.get())
    {
        check(::Server->is_running() == false)
        ::Server.reset();
    }

    check(::Server.get() == nullptr)

    return;
}

Jafg::LReStCli& Jafg::LReStCli::Get(std::string&& Pattern, ReST::LCallback&& Callback) noexcept
{
    return this->Get(std::move(Pattern), LString{}, std::move(Callback));
}

Jafg::LReStCli& Jafg::LReStCli::Get(std::string&& Pattern, LString&& Help, ReST::LCallback&& Callback) noexcept
{
    check(Tasks::IsOnMasterThread())

    this->GetRegisteredPatterns.emplace_back(std::move(Pattern), std::move(Help), std::move(Callback));

    if (::Server.get() == nullptr)
    {
        return *this;
    }

    this->RegisterPattern(--this->GetRegisteredPatterns.end());
    return *this;
}

bool Jafg::LReStCli::IsServerRunning() const noexcept
{
    return ::Server.get() && ::Server->is_running();
}

void Jafg::LReStCli::OnStop(const ERunnableStopReason::Type InType)
{
    LOG_VERBOSE(LogReST, "Stopping ReSTCli.")
    LRunnable::OnStop(InType);
    if (::Server.get() && ::Server->is_running())
    {
        ::Server->stop();
    }
    return;
}

void Jafg::LReStCli::RegisterPattern(TArray<LReSTPattern>::iterator It)
{
    check(::Server.get())
    check(It->Callback.IsValid())
    ::Server->Get(It->Pattern, [Callback = std::move(It->Callback)](httplib::Request const& Req, httplib::Response& Res)
    {
        ReST::LRequest Request{ &Req };
        ReST::LResponse Response{ &Res };
        Callback(Request, &Response);
    });
    check(It->Callback.IsValid() == false)

    LOG_VERBOSE(LogReST, "Registered ReSTCli pattern [{}] to server backend.", It->Pattern)

    return;
}

void Jafg::LReStCli::InitializeDeferred()
{
    LOG_VERBOSE(LogReST, "Initializing deferred ReSTCli patterns...")
    for (auto It{this->GetRegisteredPatterns.begin()}; It != this->GetRegisteredPatterns.end(); ++It)
    {
        if (It->Callback.IsValid())
        {
            this->RegisterPattern(It);
        }
    }
    return;
}

#endif /* JAFG_WITH_REST_CLS */
