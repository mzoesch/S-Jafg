// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/FrontendSubsystem.h"
#include "Rhi/TextureView.h"
#include "Rhi/Texture2.h"
#include "TextureSubsystem.generated.h"

namespace Jafg
{

struct LTextureView;

//# A texture that was loaded by the program and may be used across many different widgets.
DECLARE_JAFG_CLASS()
class JTextureSubsystem : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JTextureSubsystem)

public:

    virtual void Initialize(LSubsystemCollection& Collection) override;

    //# Removes all loaded textures that are not referenced anymore.
    void PurgeUnused() noexcept;

    LTextureView const& GetTextureView(LStringView Name) const noexcept;
    void RefetchingTextureViews();

    FORCEINLINE auto GetLoadedTextureCount() const noexcept { return this->Textures.size(); }
    FORCEINLINE auto const& GetTextures() const noexcept { return this->Textures; }

    ENGINE_API LTexture2Ref FromFile(LPath const& Path
        , LTexture2::HostInfo HostCreateInfo
        , LTexture2::DeviceInfo DeviceCreateInfo
        , ETexture2State State = ETexture2StateBits::Device
        );

    ENGINE_API LTexture2Ref FromTextureViewIdentifier(LStringView TextureView, ETexture2State State = ETexture2StateBits::Device);
    ENGINE_API LTexture2Ref FromTextureView(LTextureView const& View, ETexture2State State = ETexture2StateBits::Device);

private:

    TArray<LTextureView> TextureViews;
    std::unordered_map<LPath, std::shared_ptr<LTexture2>> Textures;
};

} /* ~Namespace Jafg */
