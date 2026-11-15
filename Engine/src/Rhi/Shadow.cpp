// Copyright mzoesch. All rights reserved.

#include "Rhi/Shadow.h"
#include "Rhi/NodeRenderInfo.h"
#include "Framework/Frontend.h"

namespace
{

//# Field of view limits of spot light shadows. Wider cones are only shadowed inside of the maximum.
constexpr LWorldReal MinSpotShadowFov{ 0.0175f }; /* ~1 deg */
constexpr LWorldReal MaxSpotShadowFov{ 2.9671f }; /* ~170 deg */

//#
//# How far the filter kernel of the shader reaches from the looked up point, in texels: One tap of the 3x3 grid
//# plus the bilinear footprint plus some slack. Has to match _sample_shadow_view in the shader.
//#
constexpr f32 KernelMarginTexels{ 2.5f };

//# The near plane of perspective shadow views. Too close costs depth precision, too far clips nearby casters.
NODISCARD constexpr LWorldReal GetPerspectiveNear(LWorldReal Far) noexcept
{
    return maths::clamp(Far * static_cast<LWorldReal>(0.002f), static_cast<LWorldReal>(0.02f), static_cast<LWorldReal>(0.25f));
}

//# The far plane of perspective shadow views. Nothing beyond the range of a light can be lit.
NODISCARD constexpr LWorldReal GetPerspectiveFar(Jafg::LLight const& Light, Jafg::LShadowSettings const& Settings) noexcept
{
    return Light.Range > 0.0f ? static_cast<LWorldReal>(Light.Range) : static_cast<LWorldReal>(Settings.UnlimitedRangeFar);
}

} /* ~Namespace <Anonymous> */

void Jafg::LShadowAtlas::Initialize(CreateInfo const& Info)
{
    auto& Frontend{Info.Frontend};
    auto& Settings{Info.Settings};

    check(Settings.AtlasExtent > 0 && Settings.TileExtent > 0 && Settings.CubeFaceExtent > 0)
    check(Settings.TileExtent <= Settings.AtlasExtent && Settings.CubeFaceExtent <= Settings.TileExtent)
    check(Settings.AtlasExtent % Settings.TileExtent == 0 && "The tiles have to fill the atlas exactly.")
    check(Settings.TileExtent % Settings.CubeFaceExtent == 0 && "The cube faces have to fill a tile exactly.")

    this->Extent = Settings.AtlasExtent;
    this->TileExtent = Settings.TileExtent;
    this->CubeFaceExtent = Settings.CubeFaceExtent;
    this->TilesPerRow = Settings.AtlasExtent / Settings.TileExtent;
    //# The pipelines that render into the atlas are created with the preferred depth format as well.
    this->Format = Frontend.Vk_GetPreferredDepthFormat();
    this->Reset();

    this->ImageView = nullptr;
    this->Image = Frontend.Vk_CreateDeviceLocalImage({
        .imageType = vk::ImageType::e2D,
        .format = this->Format,
        .extent = vk::Extent3D{this->Extent, this->Extent, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
        });
    this->ImageView = rhi::vk_build(Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
        .image = *this->Image,
        .viewType = vk::ImageViewType::e2D,
        .format = this->Format,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eDepth,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });
}

std::optional<vk::Offset2D> Jafg::LShadowAtlas::AllocateTile(Allocator& Alloc) const noexcept
{
    if (Alloc.NextTile >= this->TilesPerRow * this->TilesPerRow)
    {
        return {};
    }
    u32 const Tile{Alloc.NextTile++};
    return vk::Offset2D{
        static_cast<i32>((Tile % this->TilesPerRow) * this->TileExtent),
        static_cast<i32>((Tile / this->TilesPerRow) * this->TileExtent),
        };
}

std::optional<vk::Offset2D> Jafg::LShadowAtlas::AllocateFace(Allocator& Alloc) const noexcept
{
    if (Alloc.FreeFaces.empty())
    {
        auto Tile{this->AllocateTile(Alloc)};
        if (!Tile)
        {
            return {};
        }
        //# Split the tile into faces. Pushed in reverse, so that they are handed out in row major order.
        u32 const FacesPerRow{this->TileExtent / this->CubeFaceExtent};
        for (u32 Idx{FacesPerRow * FacesPerRow}; Idx-- > 0;)
        {
            Alloc.FreeFaces.emplace_back(vk::Offset2D{
                Tile->x + static_cast<i32>((Idx % FacesPerRow) * this->CubeFaceExtent),
                Tile->y + static_cast<i32>((Idx / FacesPerRow) * this->CubeFaceExtent),
                });
        }
    }
    auto Face{Alloc.FreeFaces.back()};
    Alloc.FreeFaces.pop_back();
    return Face;
}

void Jafg::LShadowAtlas::AddView(View V, SSBO::PbrShadowView DeviceView)
{
    f32 const InvExtent{1.0f / static_cast<f32>(this->Extent)};
    DeviceView.AtlasRect = LVec4F{
        static_cast<f32>(V.Offset.x) * InvExtent,
        static_cast<f32>(V.Offset.y) * InvExtent,
        static_cast<f32>(V.Extent) * InvExtent,
        static_cast<f32>(V.Extent) * InvExtent,
        };
    DeviceView.AtlasTexture = this->BindlessIndex;
    DeviceView.AtlasTexelSize = InvExtent;
    DeviceView.ViewProj = V.ProjMatrix * V.ViewMatrix;

    this->Views.emplace_back(std::move(V));
    this->DeviceViews.emplace_back(DeviceView);
}

void Jafg::LShadowAtlas::Gather(TArray<LLight const*> const& Lights, LShadowSettings const& Settings, CameraInfo const& Camera)
{
    check(this->IsInitialized())

    this->Reset();

    Allocator Alloc;

    //# Directional lights first. They usually light most of what is seen.
    for (bool const bDirectionalPass: {true, false})
    {
        for (LLight const* Light: Lights)
        {
            check(Light)

            if (!Light->bEnabled || !Light->bCastShadows || Light->Intensity <= 0.0f || !SupportsShadows(Light->Type))
            {
                continue;
            }
            if (bDirectionalPass != (Light->Type == ELight::Directional))
            {
                continue;
            }

            //# Lights get all of their tiles or none at all. A partial cube or cascade set would be wrong.
            Allocator const Backup{Alloc};
            std::size_t const First{this->Views.size()};
            bool bAllocated{};
            switch (Light->Type)
            {
            case ELight::Directional: bAllocated = this->GatherDirectional(*Light, Settings, Camera, Alloc); break;
            case ELight::Spot:        bAllocated = this->GatherSpot(*Light, Settings, Alloc);                break;
            case ELight::Point:
            case ELight::Rect:        bAllocated = this->GatherCube(*Light, Settings, Alloc);                break;
            }
            if (!bAllocated)
            {
                check(this->Views.size() == First)
                Alloc = Backup;
                continue;
            }

            check(this->Views.size() > First)
            this->Assignments.emplace_back(Light, LightViews{
                .First = static_cast<rhi::resource_index_t>(First),
                .Count = static_cast<u32>(this->Views.size() - First),
                });
        }
    }

    check(this->Views.size() == this->DeviceViews.size())
}

bool Jafg::LShadowAtlas::GatherDirectional(LLight const& Light, LShadowSettings const& Settings, CameraInfo const& Camera, Allocator& Alloc)
{
    u32 const Count{maths::clamp(Settings.CascadeCount, 1u, MaxCascades)};

    std::array<vk::Offset2D, MaxCascades> Offsets;
    for (u32 Idx{0}; Idx < Count; ++Idx)
    {
        auto Offset{this->AllocateTile(Alloc)};
        if (!Offset)
        {
            return false;
        }
        Offsets[Idx] = *Offset;
    }

    auto const& Eye{Camera.Eye};

    //# The basis of the camera frustum.
    LWorldVec3 const Front{maths::normalize(Eye.front)};
    LWorldVec3 const Right{maths::normalize(maths::cross(Front, Eye.up))};
    LWorldVec3 const Up{maths::cross(Right, Front)};
    LWorldReal const TanHalfHeight{static_cast<LWorldReal>(maths::tan(Eye.vert_fov * static_cast<LWorldReal>(0.5f)))};
    LWorldReal const TanHalfWidth{TanHalfHeight * static_cast<LWorldReal>(Camera.Aspect)};

    LWorldReal const Near{maths::max(Eye.near_frustum, static_cast<LWorldReal>(0.01f))};
    LWorldReal const Far{maths::max(maths::min(Eye.far_frustum, static_cast<LWorldReal>(Settings.DirectionalShadowDistance)), Near * 2.0f)};
    LWorldReal const Lambda{maths::clamp(static_cast<LWorldReal>(Settings.CascadeSplitLambda), static_cast<LWorldReal>(0.0f), static_cast<LWorldReal>(1.0f))};
    LWorldReal const CasterDistance{maths::max(static_cast<LWorldReal>(Settings.DirectionalCasterDistance), static_cast<LWorldReal>(0.0f))};

    //#
    //# The light space only rotates with the light. Snapping the cascades to its texel grid makes them move in
    //# whole texels when the camera moves, which keeps the edges of the shadows from shimmering.
    //#
    LWorldVec3 const LightDirection{Light.GetDirection()};
    LWorldVec3 const LightUp{Light.GetUp()};
    LMat4F const LightRotation{maths::look_at(maths::zero_vector<LWorldVec3>, LightDirection, LightUp)};

    LWorldReal SplitNear{Near};
    for (u32 Idx{0}; Idx < Count; ++Idx)
    {
        LWorldReal const T{static_cast<LWorldReal>(Idx + 1) / static_cast<LWorldReal>(Count)};
        LWorldReal const Logarithmic{Near * maths::pow(Far / Near, T)};
        LWorldReal const Uniform{Near + (Far - Near) * T};
        LWorldReal const SplitFar{Idx + 1 == Count ? Far : Uniform + (Logarithmic - Uniform) * Lambda};

        //#
        //# Bounding sphere of the slice of the camera frustum. Its radius does not change when the camera moves or
        //# rotates, so neither does the size of a texel.
        //#
        std::array<LWorldVec3, 8> Corners;
        LWorldVec3 Center{maths::zero_vector<LWorldVec3>};
        for (std::size_t Corner{0}; Corner < Corners.size(); ++Corner)
        {
            LWorldReal const Depth{(Corner & 0b100) ? SplitFar : SplitNear};
            LWorldReal const X{(Corner & 0b001) ? static_cast<LWorldReal>(1.0f) : static_cast<LWorldReal>(-1.0f)};
            LWorldReal const Y{(Corner & 0b010) ? static_cast<LWorldReal>(1.0f) : static_cast<LWorldReal>(-1.0f)};
            Corners[Corner] = Eye.translation + Front * Depth + Right * (X * Depth * TanHalfWidth) + Up * (Y * Depth * TanHalfHeight);
            Center += Corners[Corner];
        }
        Center /= static_cast<LWorldReal>(Corners.size());
        LWorldReal Radius{};
        for (auto const& Corner: Corners)
        {
            Radius = maths::max(Radius, maths::distance(Center, Corner));
        }
        //# Rounded up, so that float noise does not change the size of the texels from frame to frame.
        Radius = maths::ceil(Radius * static_cast<LWorldReal>(16.0f)) / static_cast<LWorldReal>(16.0f);

        LWorldReal const TexelWorldSize{Radius * 2.0f / static_cast<LWorldReal>(this->TileExtent)};

        LVec4F const LightSpaceCenter{LightRotation * LVec4F{LVec3F{Center}, 1.0f}};
        f32 const CenterX{static_cast<f32>(maths::floor(LightSpaceCenter.x / TexelWorldSize) * TexelWorldSize)};
        f32 const CenterY{static_cast<f32>(maths::floor(LightSpaceCenter.y / TexelWorldSize) * TexelWorldSize)};
        //# The view looks down the negative z-axis. Extends towards the light to pick up casters outside the slice.
        f32 const CenterDepth{-LightSpaceCenter.z};
        f32 const R{static_cast<f32>(Radius)};

        LMat4F Proj{glm::orthoRH_ZO(CenterX - R, CenterX + R, CenterY - R, CenterY + R
            , CenterDepth - R - static_cast<f32>(CasterDistance), CenterDepth + R)};
        //# Same convention as the perspective eye of the world, see LWorld::Draw.
        Proj[1][1] *= -1.0f;

        this->AddView(View{
            .Light = &Light,
            .Eye = LWorldEye{
                .vert_fov = Eye.vert_fov,
                .near_frustum = static_cast<LWorldReal>(0.0f),
                .far_frustum = Radius * 2.0f + CasterDistance,
                .translation = Center - LightDirection * (Radius + CasterDistance),
                .front = LightDirection,
                .up = LightUp,
                },
            .ViewMatrix = LightRotation,
            .ProjMatrix = Proj,
            .Offset = Offsets[Idx],
            .Extent = this->TileExtent,
            }, SSBO::PbrShadowView{
            .NormalBias = Settings.NormalBias * static_cast<f32>(TexelWorldSize),
            .NormalBiasPerMeter = 0.0f,
            });

        SplitNear = SplitFar;
    }

    return true;
}

bool Jafg::LShadowAtlas::GatherSpot(LLight const& Light, LShadowSettings const& Settings, Allocator& Alloc)
{
    auto Offset{this->AllocateTile(Alloc)};
    if (!Offset)
    {
        return false;
    }

    //# The frustum encloses the outer cone.
    LWorldReal const Fov{maths::clamp(static_cast<LWorldReal>(Light.OuterConeAngle) * 2.0f, MinSpotShadowFov, MaxSpotShadowFov)};
    LWorldReal const Far{::GetPerspectiveFar(Light, Settings)};
    LWorldReal const Near{::GetPerspectiveNear(Far)};

    LWorldEye const Eye{
        .vert_fov = Fov,
        .near_frustum = Near,
        .far_frustum = Far,
        .translation = Light.Translation,
        .front = LWorldVec3{Light.GetDirection()},
        //# Perpendicular to the direction by construction, so the basis never degenerates.
        .up = LWorldVec3{Light.GetUp()},
        };
    LMat4F Proj{maths::perspective<LWorldReal,world_qual>(Fov, 1.0f, Near, Far)};
    Proj[1][1] *= -1.0f;

    this->AddView(View{
        .Light = &Light,
        .Eye = Eye,
        .ViewMatrix = LMat4F{maths::look_at(Eye.translation, Eye.translation + Eye.front, Eye.up)},
        .ProjMatrix = Proj,
        .Offset = *Offset,
        .Extent = this->TileExtent,
        }, SSBO::PbrShadowView{
        .NormalBias = 0.0f,
        //# The world space size of a texel grows linearly with the distance to the light.
        .NormalBiasPerMeter = Settings.NormalBias
            * 2.0f * static_cast<f32>(maths::tan(Fov * 0.5f)) / static_cast<f32>(this->TileExtent),
        });

    return true;
}

bool Jafg::LShadowAtlas::GatherCube(LLight const& Light, LShadowSettings const& Settings, Allocator& Alloc)
{
    //# Rect lights only emit from their front face. The face behind them would never be looked up.
    u32 const Count{Light.Type == ELight::Rect ? 5u : 6u};

    std::array<vk::Offset2D, 6> Offsets;
    for (u32 Idx{0}; Idx < Count; ++Idx)
    {
        auto Offset{this->AllocateFace(Alloc)};
        if (!Offset)
        {
            return false;
        }
        Offsets[Idx] = *Offset;
    }

    //#
    //# The basis of the light. Up is derived exactly as in the shader, which picks the face by these axes.
    //# @see _sample_shadow in Content/Shaders/include/Jafg/Pbr.slang
    //#
    LWorldVec3 const Forward{Light.GetDirection()};
    LWorldVec3 const Right{Light.GetRight()};
    LWorldVec3 const Up{maths::cross(Right, Forward)};
    std::array<LWorldVec3, 6> const Axes{Right, -Right, Up, -Up, Forward, -Forward};
    std::array<LWorldVec3, 6> const Ups{Up, Up, Forward, Forward, Up, Up};

    //#
    //# Slightly wider than 90 deg. The shader only looks up points whose dominant axis is the one of the face, which
    //# then project into the inner part of the tile. The filter kernel never reaches beyond the tile that way.
    //#
    f32 const Res{static_cast<f32>(this->CubeFaceExtent)};
    LWorldReal const Fov{static_cast<LWorldReal>(2.0f * maths::atan(Res / (Res - 2.0f * KernelMarginTexels)))};
    LWorldReal const Far{::GetPerspectiveFar(Light, Settings)};
    LWorldReal const Near{::GetPerspectiveNear(Far)};

    LMat4F Proj{maths::perspective<LWorldReal,world_qual>(Fov, 1.0f, Near, Far)};
    Proj[1][1] *= -1.0f;

    f32 const NormalBiasPerMeter{Settings.NormalBias * 2.0f * static_cast<f32>(maths::tan(Fov * 0.5f)) / Res};

    for (u32 Idx{0}; Idx < Count; ++Idx)
    {
        LWorldEye const Eye{
            .vert_fov = Fov,
            .near_frustum = Near,
            .far_frustum = Far,
            .translation = Light.Translation,
            .front = Axes[Idx],
            .up = Ups[Idx],
            };
        this->AddView(View{
            .Light = &Light,
            .Eye = Eye,
            .ViewMatrix = LMat4F{maths::look_at(Eye.translation, Eye.translation + Eye.front, Eye.up)},
            .ProjMatrix = Proj,
            .Offset = Offsets[Idx],
            .Extent = this->CubeFaceExtent,
            }, SSBO::PbrShadowView{
            .NormalBias = 0.0f,
            .NormalBiasPerMeter = NormalBiasPerMeter,
            });
    }

    return true;
}

void Jafg::LShadowAtlas::Render(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info, View const& View)> What) const
{
    check(!!What)
    check(this->IsInitialized())
    check(this->HasViews())

    LRenderInfo RenderInfo{Info};
    auto& Surface{RenderInfo.Surface};

    constexpr vk::ImageSubresourceRange DepthRange{
        .aspectMask = vk::ImageAspectFlagBits::eDepth,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
        };

    //# The previous content is never needed. Waits for the fragment shaders of the last frame that sampled it.
    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eFragmentShader,
        .srcAccessMask = {},
        .dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
        .dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eDepthAttachmentOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = *this->Image,
        .subresourceRange = DepthRange,
        });

    constexpr vk::ClearValue ClearDepth{.depthStencil = vk::ClearDepthStencilValue{.depth = 1.0f, .stencil = 0}};
    vk::RenderingAttachmentInfo DepthAttachmentInfo{
        .imageView   = *this->ImageView,
        .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eStore,
        .clearValue  = ClearDepth,
        };

    RenderInfo.CommandBuffer.beginRendering({
        .renderArea = {.offset = {0, 0}, .extent = {this->Extent, this->Extent}},
        .layerCount = 1,
        .colorAttachmentCount = 0,
        .pColorAttachments = nullptr,
        .pDepthAttachment = &DepthAttachmentInfo,
        .pStencilAttachment = nullptr,
        });

    for (View const& V: this->Views)
    {
        RenderInfo.VkViewport = vk::Viewport{
            .x = static_cast<f32>(V.Offset.x), .y = static_cast<f32>(V.Offset.y),
            .width = static_cast<f32>(V.Extent), .height = static_cast<f32>(V.Extent),
            .minDepth = 0.0f, .maxDepth = 1.0f,
            };
        RenderInfo.VkScissor = vk::Rect2D{
            .offset = V.Offset,
            .extent = {V.Extent, V.Extent},
            };
        RenderInfo.CommandBuffer.setViewport(0, RenderInfo.VkViewport);
        RenderInfo.CommandBuffer.setScissor(0, RenderInfo.VkScissor);
        What(RenderInfo, V);
    }

    RenderInfo.CommandBuffer.endRendering();

    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
        .srcAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader,
        .dstAccessMask = vk::AccessFlagBits2::eShaderSampledRead,
        .oldLayout = vk::ImageLayout::eDepthAttachmentOptimal,
        .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = *this->Image,
        .subresourceRange = DepthRange,
        });
}

Jafg::LShadowAtlas::LightViews Jafg::LShadowAtlas::FindViews(LLight const& Light) const noexcept
{
    for (auto const& [Assigned, Views]: this->Assignments)
    {
        if (Assigned == &Light)
        {
            return Views;
        }
    }
    return {};
}
