// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Surface.h"

namespace Jafg
{

class LSurfaceProxy final
{
public:

    LSurfaceProxy() = delete;
    explicit LSurfaceProxy(LSurface* InSurface) : Proxy(InSurface) { check( this->Proxy ) }
    DEFAULT_REALLOC_OF_ANY_FORM(LSurfaceProxy)
    ~LSurfaceProxy() = default;

    // ... for the future here implement the methods to allow template specialization for the proxy.

private:

    LSurface* Proxy;
};

} /* ~Namespace Jafg */
