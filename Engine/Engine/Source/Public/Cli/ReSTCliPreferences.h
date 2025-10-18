// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClass.h"
#include "ReSTCliPreferences.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(ECxxClassFlags::Config, ECxxClassFlags::Singleton)
class JReSTCliPreferences final : public JCxxClass
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JReSTCliPreferences)

public:

    //#
    //# Whether to always disable the ReST CLI. The server will never be started
    //# even if appropriate program arguments are provided.
    //#
    CLASS_FIELD(Config)
    bool bAlwaysDisable{ false };

    //# Whether to automatically start the ReST CLI server when the engine starts.
    CLASS_FIELD(Config)
    bool bAutoStart{ false };

    //# Default host to bind to.
    CLASS_FIELD(Config)
    LString Host{ "0.0.0.0" };

    //# Default port to start.
    CLASS_FIELD(Config)
    i32 Port{ 8080 };

    CLASS_FIELD(Config)
    LSize KeepAliveMaxRequests{ 100 };
    CLASS_FIELD(Config)
    f64 KeepAliveTimeoutInSeconds{ 5.0 };
    CLASS_FIELD(Config)
    f64 ReadTimeoutInSeconds{ 5.0 };
    CLASS_FIELD(Config)
    f64 WriteTimeoutInSeconds{ 5.0 };
    CLASS_FIELD(Config)
    f64 IdleIntervalInSeconds{ 0.0 };

    CLASS_FIELD(Config)
    size_t PayLoadMaxLength{ std::numeric_limits<size_t>::max() };

    CLASS_FIELD(Config)
    bool TcpNoDelay{ false };
    CLASS_FIELD(Config)
    bool Ipv6_v6Only{ false };
};

} /* ~Namespace Jafg */
