// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/ApplicationInstanceSubsystem.h"
#include "Rhi/Texture2.h"
#include "MyWorld/CommonTypes.h"
#include "MaterialSubsystem.generated.h"

namespace Jafg
{

class LTexture2;

DECLARE_JAFG_CLASS()
class JMaterialSubsystem final : public JApplicationInstanceSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JMaterialSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

public:

    FORCEINLINE auto CalculateSpecificTexturePointOnAtlas(const LTextureIndex InTextureIndex) const -> LPoint;
    FORCEINLINE auto GetCurrentTextureWidth() const -> int32 { return this->CurrentTextureWidth; }

    FORCEINLINE auto HasAtlas() const -> bool { return this->Atlas.GetFirstMipMap().Bulk.IsAllocated(); }
    FORCEINLINE auto GetAtlas() const -> const LTexture2& { check( this->HasAtlas() ) return this->Atlas; }

    FORCEINLINE auto GetDomainWidth() const -> uint32 { return this->Atlas.GetFirstMipMap().Size.X / this->CurrentTextureWidth; }

private:

    void ReloadAllTextures(void);
    void ClearAllTextures(void);

    void LoadAllTextures(void);
    void CreateAtlas(const TdhArray<LTexture2>& Textures);

    /**
     * The texture width to use for all textures. Is guaranteed to be a power of two.
     * Lower resolutions will be scaled up.
     */
    int32 CurrentTextureWidth = 0;

    LTexture2 Atlas = { };
};

LPoint JMaterialSubsystem::CalculateSpecificTexturePointOnAtlas(const LTextureIndex InTextureIndex) const
{
    const uint32 Width = this->Atlas.GetFirstMipMap().Size.X;

    const uint32 X = (InTextureIndex * this->CurrentTextureWidth) % Width;
    const uint32 Y = ((InTextureIndex * this->CurrentTextureWidth) / Width) * this->CurrentTextureWidth;

    return LPoint(X, Y);
}

} /* ~Namespace Jafg */
