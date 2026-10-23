// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/PersonaControllerComponent.h"
#include "Rhi/Material.h"
#include "DebugDrawerComponent.generated.h"

namespace Jafg
{

#if JAFG_WITH_EDITOR

namespace SSBO
{

struct Ray: rhi::ssbo_template<Ray>
{
    constexpr Ray(LVec3F Origin, LVec3F End, LColor Tint) noexcept: Origin{Origin}, End{End}, Tint{Tint} {}

    LVec3F Origin;
    f32 _pad0{1.0f};
    LVec3F End;
    LColor Tint;
};
static_assert(rhi::ssbo<Ray>);

} /* ~Namespace SSBO */

DECLARE_JAFG_CLASS()
class ENGINE_API ADebugDrawerComponent : public APersonaControllerComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(ADebugDrawerComponent)
    {
        this->bTick = true;
        this->SetShouldRender(true);
    }

public:

    static constexpr u64 MaxLineCount{16'384};
    static constexpr f32 OneTimeDraw{0.0f};

    static constexpr u64 ShaderSpace{0uz};
    static constexpr u64 DebugLinesIndex{0uz};
    static constexpr u64 ViewProjIndex{1uz};

    virtual void OnAttach(AActor& InOwner) override;
    virtual void ParentTick(f32 Dt) override;
    virtual void Render(LActorRenderInfo const& Info) const override;

    virtual void OnDeferredRender(LActorRenderInfo const& Info) const;

    struct LineCreateInfo final
    {
        LColor Tint;
        f32 Duration{ OneTimeDraw };
        LWorldLine3 Value;
        LWorldLine3 const* operator->() const noexcept { return &this->Value; }
    };
    void AddLine(LineCreateInfo Line) noexcept { this->Lines.emplace_back(std::move(Line)); }
    //# Useful for consistent one time draws when draw orders are not guaranteed.
    void AddLineNextTick(LineCreateInfo Line) noexcept { this->NextLines.emplace_back(std::move(Line)); }

    struct RayCreateInfo final
    {
        LColor Tint;
        f32 Duration{ OneTimeDraw };
        LWorldMagRay3 Value;
        LWorldMagRay3 const* operator->() const noexcept { return &this->Value; }
    };
    void AddRay(RayCreateInfo Ray) noexcept { this->Rays.emplace_back(std::move(Ray)); }
    //# Useful for consistent one time draws when draw orders are not guaranteed.
    void AddRayNextTick(RayCreateInfo Ray) noexcept { this->NextRays.emplace_back(std::move(Ray)); }

    struct AabbCreateInfo final
    {
        LColor Tint;
        f32 Duration{ OneTimeDraw };
        LWorldAabb3 Value;
        LWorldAabb3 const* operator->() const noexcept { return &this->Value; }
    };
    void AddAabb(AabbCreateInfo Aabb) noexcept { this->Aabbs.emplace_back(std::move(Aabb)); }
    //# Useful for consistent one time draws when draw orders are not guaranteed.
    void AddAabbNextTick(AabbCreateInfo Aabb) noexcept { this->NextAabbs.emplace_back(std::move(Aabb)); }

    struct TextCreateInfo final
    {
        LColor Tint;
        f32 Duration{ OneTimeDraw };
        f32 Height;
        LWorldVec3 Location;
        LString Text;
        LWorldVec3 const* operator->() const noexcept { return &this->Location; }
    };
    void AddText(TextCreateInfo Text) noexcept { this->Texts.emplace_back(std::move(Text)); }
    //# Useful for consistent one time draws when draw orders are not guaranteed.
    void AddTextNextTick(TextCreateInfo Text) noexcept { this->NextTexts.emplace_back(std::move(Text)); }

private:

    void RenderRays(LActorRenderInfo const& Info) const;
    void RenderTexts(LActorRenderInfo const& Info) const;

    mutable TArray<LineCreateInfo> Lines;
    TArray<LineCreateInfo> NextLines;
    mutable TArray<RayCreateInfo> Rays;
    TArray<RayCreateInfo> NextRays;
    mutable TArray<AabbCreateInfo> Aabbs;
    TArray<AabbCreateInfo> NextAabbs;
    mutable TArray<TextCreateInfo> Texts;
    TArray<TextCreateInfo> NextTexts;

    LMaterialInstanceRef RayInstance;
    rhi::frame_array<rhi::mapped_device_buffer> RayBuffers;
    rhi::frame_array<rhi::mapped_device_buffer> RayViewBuffers;

    LMaterialInstanceRef TextMaterialInstance;
};

#endif /* JAFG_WITH_EDITOR */

} /* ~Namespace Jafg */
