// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

class Surface
{
public:

    virtual ~Surface() = default;

    virtual void OnUpdate() = 0;

    NODISCARD virtual int32 GetWidth() const = 0;
    NODISCARD virtual int32 GetHeight() const = 0;

              virtual void SetVSync(const bool bEnabled) = 0;
    NODISCARD virtual bool IsVSync() const = 0;

    template<class T>
    NODISCARD T* As() { return static_cast<T*>(this); }
};
