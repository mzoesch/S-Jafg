// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

class LEye;
struct LRenderInfo;

class ENGINE_API LActorComponent
{
public:

    constexpr LActorComponent() noexcept = default;
    virtual ~LActorComponent() noexcept = default;

    constexpr void SetShouldRender(bool b) noexcept { this->bRender = b; }
    constexpr bool ShouldRender() const noexcept { return this->bRender; }
    virtual void Render(LRenderInfo const& Info, LEye const& Eye) noexcept {}

private:

    bool bRender:1{};
};

} /* ~Namespace Jafg */
