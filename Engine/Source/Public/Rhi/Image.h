// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Texture2.h"

namespace Jafg
{

struct LImage
{
    constexpr LImage() noexcept = default;
    explicit LImage(std::shared_ptr<LTexture2> InTexture) noexcept : Texture{ std::move(InTexture) } {}
    ~LImage() noexcept = default;

    FORCEINLINE bool HasTexture() const noexcept { return this->Texture.get() != nullptr; }
    FORCEINLINE auto const& GetTexture() const noexcept { check( this->HasTexture() ) return *this->Texture; }

    FORCEINLINE std::shared_ptr<LTexture2> GetNewHandle() const noexcept
    {
        check( this->HasTexture() )
        return this->Texture;
    }

private:

    std::shared_ptr<LTexture2> Texture;
};

} /* ~Namespace Jafg */
