// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

/** Interface for a generic surface that the RHI may use to draw on. */
class ENGINE_API Surface
{
public:

    virtual ~Surface() = default;

    virtual void Initialize()       = 0;
    virtual void OnClear()          = 0;
    virtual void OnUpdate()         = 0;
    virtual void TearDown()         = 0;

    virtual void PollInputs()       = 0;
    virtual void PollEvents()       = 0;

    virtual void SetInputMode(bool bShowCursor) = 0;

    NODISCARD virtual auto GetWidth() const -> int32                    = 0;
    NODISCARD virtual auto GetHeight() const -> int32                   = 0;
    NODISCARD virtual auto GetDimensions() const -> TIntVector2<int32>  = 0;

              virtual void SetVSync(const bool bEnabled) = 0;
    NODISCARD virtual bool IsVSync() const               = 0;

    template<class T = Surface>
    NODISCARD T* As() { return static_cast<T*>(this); }
};

}
