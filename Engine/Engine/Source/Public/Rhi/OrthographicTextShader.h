// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/EngineShader.h"
#include "Widgets/Whitespace.h"
#include "Widgets/TextBoxForward.h"

namespace Jafg
{

struct LCharacter final
{
    u32 Handle{ 0 };
    Lu32Vector2 Size;
    Li32Vector2 Bearing;
    Li64Vector2 Advance;
};

class LOrthographicTextShader : public LEngineShader
{
public:

    typedef LEngineShader Super;
    typedef std::map<u8, LCharacter> LCharacterMap;

    virtual bool Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants) override { return true; }
    virtual void UpdateViewportUniforms(const LViewport& Context) override {}
    virtual void OnFree() override {}

    ENGINE_API void Draw
    (
        const LViewport& Context,
        const LVector2D& Size,
        const LVector2D& TopLeft,
        const LPadding& Padding,
        const LVector2D& TextDesiredSize,
        const ETextHAlign::Type TextHAlign,
        const ETextVAlign::Type TextVAlign,
        const Lal::LColor& TextColor,
        const f64 TextScale,
        const LString& Content
    ) const {}

    NODISCARD
    FORCEINLINE f32 GetApproxBearingHeight(const f32 InScale) const { check( this->ApproxBearingHeight > 0.0f ) return this->ApproxBearingHeight * InScale; }

    FORCEINLINE void SetFontPath(const LEnginePath& InFontPath) noexcept { this->FontPath = InFontPath; }
    FORCEINLINE const LEnginePath& GetFontPath() const noexcept { return this->FontPath; }

    FORCEINLINE const LCharacterMap& GetCharacters() const noexcept { return this->Characters; }
    
private:

    LEnginePath FontPath{ LEnginePath{EEnginePaths::Fonts, "CascadiaMono/ttf/CascadiaMono.ttf"} };
    LCharacterMap Characters;

    f32 ApproxBearingHeight{ -1.0f };

    u32 Vao{ 0x0u };
    u32 Vbo{ 0x0u };
};

} /* ~Namespace Jafg */
