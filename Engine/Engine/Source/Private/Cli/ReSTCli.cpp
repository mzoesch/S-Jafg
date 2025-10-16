// Copyright mzoesch. All rights reserved.

#include "Cli/ReSTCli.h"

#if JAFG_WITH_REST_CLS

#include "Async/TaskUtility.h"

#define CPPHTTPLIB_THREAD_POOL_COUNT                                    1
#define CPPHTTPLIB_NO_EXCEPTIONS                                        1
#define CPPHTTPLIB_ALLOW_LF_AS_LINE_TERMINATOR                          1
#include "httplib.h"

namespace
{

//# There can only ever be one server instance.
TUnique<httplib::Server> Server;

} /* ~Namespace <Anonymous> */

void Jafg::ReST::LResponse::SetStatusCode(i32 InStatusCode) noexcept
{
    static_cast<httplib::Response*>(this->Pimpl)->status = InStatusCode;
}

void Jafg::ReST::LResponse::AddHeader(std::string&& InHeader, std::string&& InValue) noexcept
{
    static_cast<httplib::Response*>(this->Pimpl)->set_header(std::move(InHeader), std::move(InValue));
}

void Jafg::ReST::LResponse::SetContent(std::string&& Content, std::string&& ContentType) noexcept
{
    static_cast<httplib::Response*>(this->Pimpl)->set_content(std::move(Content), std::move(ContentType));
}

Jafg::LReStCli::LReStCli() noexcept
    : LRunnable("ReSTCli")
{
    return;
}

Jafg::ETaskExit::Type Jafg::LReStCli::Initialize()
{
    LOG_VERBOSE(LogReST, "Initializing ReSTCli.")

    ::Server = std::make_unique<httplib::Server>();
    check( ::Server.get() )

    if constexpr (IS_COMPILED_LOG(LogReST, Verbose))
    {
        ::Server->set_logger([](httplib::Request const& Req, httplib::Response const& Res)
        {
            std::stringstream ss;

            ss << Req.method << " " << Req.path << " -> " << Res.status;
            if (Req.method == "POST" || Req.method == "PUT")
            {
                ss << " (" << Req.body.size() << " bytes)";
            }

            LOG_VERBOSE(LogReST, "{}", ss.str())

            return;
        });
    }

    if constexpr (IS_COMPILED_LOG(LogReST, Verbose))
    {
        ::Server->set_pre_compression_logger([](httplib::Request const& Req, httplib::Response const& Res)
        {
            std::stringstream ss;

            ss << "Pre-compressed response for " << Req.method << " " << Req.path << " -> " << Res.status;
            if (Req.method == "POST" || Req.method == "PUT")
            {
                ss << " (" << Req.body.size() << " bytes)";
            }

            LOG_VERBOSE(LogReST, "{}", ss.str())

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
        Res.set_header("Access-Control-Allow-Origin", "*");
        Res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        Res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");

        Res.status = httplib::NoContent_204;

        return;
    });

    ::Server->set_post_routing_handler([](httplib::Request const&, httplib::Response& Res)
    {
        Res.set_header("Access-Control-Allow-Origin", "*");
        Res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        Res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");

        return;
    });

    ::Server->Get("/docs", [](httplib::Request const& Req, httplib::Response& Res)
    {
        check( ::Server.get() )
        // Fork the repo and add visitors...
        Res.set_content("{}", "application/json");
        return;
    });


    return LRunnable::Initialize();
}

Jafg::ETaskExit::Type Jafg::LReStCli::Run()
{
    check( Tasks::IsOnMasterThread() == false )
    check( Tasks::IsOnReSTCliThread() )

    LOG_VERBOSE(LogReST, "Starting ReSTCli.")

    check( ::Server->is_running() == false )

    ::Server->listen("0.0.0.0", 8080);

    check( ::Server->is_running() == false )
    LOG_VERBOSE(LogReST, "ReSTCli stopped.")
    return ETaskExit::Success;
}

void Jafg::LReStCli::Exit()
{
    check( Tasks::IsOnMasterThread() == false )
    check( Tasks::IsOnReSTCliThread() )

    LRunnable::Exit();

    check( ::Server.get() && ::Server->is_running() == false )
    ::Server.reset();

    return;
}

Jafg::LReStCli& Jafg::LReStCli::Get(std::string&& Pattern, ReST::LCallback&& Callback) noexcept
{
    check( ::Server.get() )
    ::Server->Get(std::move(Pattern), [Callback = std::move(Callback)](httplib::Request const& Req, httplib::Response& Res)
    {
        ReST::LRequest Request{ &Req };
        ReST::LResponse Response{ &Res };

        Callback(Request, &Response);

        return;
    });

    return *this;
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

#endif /* JAFG_WITH_REST_CLS */
