// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/EngineShader.h"
#include "Widgets/Whitespace.h"
#include "Widgets/TextBoxForward.h"

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.inl>

namespace Jafg
{

struct Character final
{
    u32     TextureId = 0;             // ID handle of the glyph texture
    glm::ivec2 Size      = glm::ivec2();  // Size of glyph
    glm::ivec2 Bearing   = glm::ivec2();  // Offset from baseline to left/top of glyph
    Jafg::LIntVector2 Advance  = Jafg::LIntVector2(); // Offset to advance to next glyph

    Character() = default;
    Character(const Character& InOther)
    {
        this->TextureId = InOther.TextureId;
        this->Size      = InOther.Size;
        this->Bearing   = InOther.Bearing;
        this->Advance   = InOther.Advance;

        return;
    }

    Character(const u32 InTextureId, const glm::ivec2& InSize, const glm::ivec2& InBearing, const Jafg::LIntVector2& InAdvance)
    {
        this->TextureId = InTextureId;
        this->Size      = InSize;
        this->Bearing   = InBearing;
        this->Advance   = InAdvance;

        return;
    }
};

class LOrthographicTextShader : public LEngineShader
{
public:

    typedef LEngineShader Super;
    typedef std::map<u8, Character> LCharacterMap;

    virtual bool Make(const LName InName, TArray<LShaderCompileTimeConstant>&& InConstants) override;
    virtual void UpdateViewportUniforms(const LViewport& Context) override;
    virtual void OnFree() override;

    ENGINE_API void Draw
    (
        const LViewport& Context,
        const LVector2& Size,
        const LVector2& TopLeft,
        const LPadding& Padding,
        const LVector2& TextDesiredSize,
        const ETextHAlign::Type TextHAlign,
        const ETextVAlign::Type TextVAlign,
        const LColor& TextColor,
        const f32 TextScale,
        const LString& Content
    ) const;

    //#
    //# The approximate height of the font when looking at the highest character.
    //#
    NODISCARD
    FORCEINLINE f32 GetApproximateHeight(const f32 InScale) const { check( this->ApproxHeight > 0.0f ) return this->ApproxHeight * InScale; }

    FORCEINLINE void SetFontPath(const LEnginePath& InFontPath) noexcept { this->FontPath = InFontPath; }
    FORCEINLINE const LEnginePath& GetFontPath() const noexcept { return this->FontPath; }

    FORCEINLINE const LCharacterMap& GetCharacters() const noexcept { return this->Characters; }
    
private:

    LEnginePath FontPath { LEnginePath{EEnginePaths::Fonts, "Core.otf"} };
    LCharacterMap Characters;

    f32 ApproxHeight { -1.0f };

    u32 Vao { 0x0u };
    u32 Vbo { 0x0u };
};

} /* ~Namespace Jafg */
