// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "Rhi/Texture2.h"
#include "MyWorld/CommonTypes.h"
#include "Rhi/Atlas2.h"
#include "MaterialSubsystem.generated.h"

namespace Jafg
{

class LTexture2;

DECLARE_JAFG_CLASS()
class JMaterialSubsystem final : public JEngineSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JMaterialSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;

public:

    FORCEINLINE auto CalculateSpecificTexturePointOnBlendOpaqueAtlas(const LTextureIndex InTextureIndex) const -> LPoint { return this->BlendOpaqueAtlas.CalculateSpecificTexturePointOnAtlas(InTextureIndex); }
    FORCEINLINE auto HasBlendOpaqueAtlas() const -> bool { return this->BlendOpaqueAtlas.GetMostSignificantMipMap().GetBulk().IsAllocated(); }
    FORCEINLINE auto GetBlendOpaqueAtlas() const -> const LAtlas2& { check( this->HasBlendOpaqueAtlas() ) return this->BlendOpaqueAtlas; }
    FORCEINLINE auto GetBlendOpaqueAtlasTexture() const -> const LTexture2& { check( this->HasBlendOpaqueAtlas() ) return this->BlendOpaqueAtlas.GetData(); }
    FORCEINLINE auto GetBlendOpaqueDomainWidth() const -> u32 { return this->BlendOpaqueAtlas.GetDomainWidth(); }

    FORCEINLINE auto CalculateSpecificTexturePointOnBlendersAtlas(const LTextureIndex InTextureIndex) const -> LPoint { return this->BlendersAtlas.CalculateSpecificTexturePointOnAtlas(InTextureIndex); }
    FORCEINLINE auto HasBlendersAtlas() const -> bool { return this->BlendersAtlas.GetMostSignificantMipMap().GetBulk().IsAllocated(); }
    FORCEINLINE auto GetBlendersAtlas() const -> const LAtlas2& { check( this->HasBlendersAtlas() ) return this->BlendersAtlas; }
    FORCEINLINE auto GetBlendersAtlasTexture() const -> const LTexture2& { check( this->HasBlendersAtlas() ) return this->BlendersAtlas.GetData(); }
    FORCEINLINE auto GetBlendersDomainWidth() const -> u32 { return this->BlendersAtlas.GetDomainWidth(); }

private:

    void ReloadAllTextures(void);
    void ClearAllTextures(void);
    void LoadAllTextures(void);

    LAtlas2 BlendersAtlas    = { };
    LAtlas2 BlendOpaqueAtlas = { };
};

} /* ~Namespace Jafg */
