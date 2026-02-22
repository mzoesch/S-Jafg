// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Texture2.h"
#include "Engine/CxxClass.h"
#include "TextureSubsystem.generated.h"

namespace Jafg
{

//# A texture that was loaded by the program and may be used across many different widgets.
DECLARE_JAFG_CLASS(ECxxClassFlags::Singleton)
class JTextureSubsystem : public JCxxClass
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JTextureSubsystem)

public:

    //# Removes all loaded textures that are not referenced anymore.
    void PurgeUnused() noexcept;

    FORCEINLINE auto GetLoadedTextureCount() const noexcept { return this->Textures.size(); }
    FORCEINLINE auto const& GetTextures() const noexcept { return this->Textures; }

    ENGINE_API LTexture2Ref FromFile(LPath const& Path
        , LTexture2::HostInfo HostCreateInfo
        , LTexture2::DeviceInfo DeviceCreateInfo
        , ETexture2State State = ETexture2StateBits::Device
        ) const;

private:

    mutable std::unordered_map<LPath, std::shared_ptr<LTexture2>> Textures;
};

} /* ~Namespace Jafg */
