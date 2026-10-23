// Copyright mzoesch. All rights reserved.

#include "Framework/DebugDrawerComponent.h"
#include "Framework/Frontend.h"
#include "User/LocalEgo.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/FontSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Rhi/ViewProj.h"
#include "User/UserPreferences.h"
#include "Rhi/VisualInstance.h"
#include "Engine/Engine.h"

void Jafg::ADebugDrawerComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);

    LOG_TRACE(LogRhi, "Allocating debug editor buffers.")
    auto& Frontend{this->GetMutableLocalEgo().GetFrontend()};
    check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)

    this->RayInstance = Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.DebugLine");
    check(this->RayInstance->Material->Template.my_shader.get_descriptor_slot(ShaderSpace, DebugLinesIndex).CxxName == SSBO::Ray::name())
    check(this->RayInstance->Material->Template.my_shader.get_descriptor_slot(ShaderSpace, ViewProjIndex).CxxName == UBO::ViewProj::name())

    this->RayBuffers = Frontend.Vk_CreateFrequentMappedBuffer(SSBO::Ray::buffer_create_info(MaxLineCount));
    this->RayViewBuffers = Frontend.Vk_CreateFrequentMappedBuffer(UBO::ViewProj::buffer_create_info());

    this->TextMaterialInstance = Frontend.GetSubsystemChecked<JMaterialSubsystem>()
        ->GetInstanceFromMaterialName("Jafg.VisualBatch"sv);
}

void Jafg::ADebugDrawerComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    auto Reduce{[Dt=static_cast<f32>(this->GetEngine().DeltaTime)](auto&& Range)
    {
        for (auto It{Range.begin()}; It != Range.end();)
        {
            It->Duration -= Dt;
            if (It->Duration <= 0.0f)
            {
                It = Range.erase(It);
            }
            else
            {
                ++It;
            }
        }
    }};

    Reduce(this->Lines);
    Reduce(this->Rays);
    Reduce(this->Aabbs);
    Reduce(this->Texts);

    this->Lines.append_range(this->NextLines);
    this->NextLines.clear();
    this->Rays.append_range(this->NextRays);
    this->NextRays.clear();
    this->Aabbs.append_range(this->NextAabbs);
    this->NextAabbs.clear();
    this->Texts.append_range(this->NextTexts);
    this->NextTexts.clear();
}

void Jafg::ADebugDrawerComponent::Render(LActorRenderInfo const& Info) const
{
    Super::Render(Info);
    Info.PostRenderDelegates.emplace_back([this](LActorRenderInfo const& Info)
    {
        this->OnDeferredRender(Info);
    });
}

void Jafg::ADebugDrawerComponent::OnDeferredRender(LActorRenderInfo const& Info) const
{
    STAT_FUNCTION()
    this->RenderRays(Info);
    this->RenderTexts(Info);
}

void Jafg::ADebugDrawerComponent::RenderRays(LActorRenderInfo const& Info) const
{
    STAT_FUNCTION()

    auto& Prefs{GetSingleton<JUserPreferences>()};
    check(this->RayInstance.get())

    if (*Prefs.EditorVisualizeAabbs)
    {
        for (auto& E: this->GetWorld().GetEmployees() | algo::views::filter([](auto& E){ return E->template IsA<AActor>(); }))
        {
            auto& A{*StaticCastChecked<AActor>(&*E)};
            LWorldAabb3 Aabb{A.GetTransformedActorAabb()};
            if (Aabb.empty())
            {
                continue;
            }
            this->Aabbs.emplace_back(*Prefs.EditorAabbVisualizationTint, OneTimeDraw, Aabb);

            if (*Prefs.EditorVisualizeTransitiveAabbs)
            {
                auto VisualizeComponentTransitive{[this,Prefs=&Prefs](this auto&& Self, ASceneComponent& Comp) -> void
                {
                    LWorldAabb3 Aabb{Comp.GetAabbForThisComponentOnly()};
                    if (!Aabb.empty())
                    {
                        this->Aabbs.emplace_back(*Prefs->EditorTransitiveAabbVisualizationTint, OneTimeDraw, Aabb.apply(Comp.GetWorldTransformSlow()));
                    }
                    for (auto& Child: Comp.GetChildren())
                    {
                        Self(*Child);
                    }
                }};
                VisualizeComponentTransitive(A.GetRootComponent());
            }
        }
    }

    if (this->Lines.empty() && this->Rays.empty() && this->Aabbs.empty())
    {
        return;
    }

    auto& Set{this->RayInstance->Vk_GetUniqueDescriptorSet(ShaderSpace, Info.Frame)};

    rhi::object_range<SSBO::Ray> DeviceRays{this->Lines.size() + this->Rays.size() + this->Aabbs.size() * 12, MaxLineCount};
    algo::for_each(this->Lines, [&DeviceRays](auto const& Line)
    {
        DeviceRays->emplace_back(Line->begin, Line->end, Line.Tint);
    });
    algo::for_each(this->Rays, [&DeviceRays](auto const& Ray)
    {
        DeviceRays->emplace_back(Ray->origin, Ray->direction * Ray->magnitude, Ray.Tint);
    });
    algo::for_each(this->Aabbs, [&DeviceRays](auto const& Aabb)
    {
        for (auto& edge: Aabb->edges())
        {
            DeviceRays->emplace_back(edge.origin, edge.direction, Aabb.Tint);
        }
    });

    DeviceRays.upload_and_update(Info.Frontend.Vk_GetDevice(), *Set, DebugLinesIndex, this->RayBuffers[Info.Frame]);
    UBO::ViewProj{Info.WorldData.proj * Info.WorldData.view}.upload_and_update(Info.Frontend.Vk_GetDevice(), *Set, ViewProjIndex, this->RayViewBuffers[Info.Frame]);

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *this->RayInstance->Material->Pipeline);
    Info.CommandBuffer.bindDescriptorSets2({
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .layout = *this->RayInstance->Material->Pipeline.pipeline_layout,
        .firstSet = 0,
        .descriptorSetCount = 1,
        .pDescriptorSets = &**Set,
        .dynamicOffsetCount = 0,
        .pDynamicOffsets = nullptr
        });
    Info.CommandBuffer.draw(2, static_cast<u32>(DeviceRays->size()), 0, 0);
}

void Jafg::ADebugDrawerComponent::RenderTexts(LActorRenderInfo const& Info) const
{
    if (this->Texts.empty())
    {
        return;
    }

    STAT_FUNCTION()
    check(this->TextMaterialInstance.get())

    auto& Prefs{GetSingleton<JUserPreferences>()};

    auto& Frontend{this->GetFrontend()};
    auto& Subsystem{*Frontend.GetSubsystemChecked<JFontSubsystem>()};
    rhi::object_range<SSBO::VisualInstance> Range{SSBO::VisualInstance::default_count,SSBO::VisualInstance::default_count};

    auto ViewProjection{Info.WorldData.proj * Info.WorldData.view};
    for (auto& Text: this->Texts)
    {
        LWorldReal Distance{maths::magnitude(Info.PerspectiveEye.translation - Text.Location)};
        if (Distance > *Prefs.EditorVisualizationMaxDebugTextRenderDistance)
        {
            continue;
        }

        LVec4F Clip{ViewProjection * LVec4F{Text.Location, 1.0f}};
        if (Clip.w <= 0.0f)
        {
            continue;
        }

        auto FontSize{Text.Height * ((Info.VkViewport.height - Info.VkViewport.y) / (2 * maths::tan(Info.PerspectiveEye.vert_fov / 2) * Distance))};
        if (FontSize < *Prefs.EditorVisualizationDebugTextFontThreshold)
        {
            continue;
        }

        /* TODO: Most definitely cache this result. This line will become most probably a bottleneck later...
         *       But can it be cached with variadic font size (based of eye translation)? */
        LVec2F Pencil{maths::zero_vector<LVec2F>};
        auto Infos{Subsystem.GetGlyphInfos(0, Text.Text, FontSize, &Pencil)};

        LVec3F Ndc{maths::xyz(Clip) / Clip.w};
        if (Ndc.z < 0.0f  || Ndc.z > 1.0f)
        {
            continue;
        }
        LVec2F Offset{
            Info.VkViewport.x + (Ndc.x * 0.5f + 0.5f) * Info.VkViewport.width,
            Info.VkViewport.y + (1.0f - (-Ndc.y * 0.5f + 0.5f)) * Info.VkViewport.height
            };
        for (auto& Glyph: Infos.GlyphInfos)
        {
            LRect2F Rect{{Offset.x + Glyph.Rect.x, Offset.y + Glyph.Rect.y}, {Glyph.Rect.z, Glyph.Rect.w}};
            if (maths::aabb(LRect2F{
                .offset = {Info.VkViewport.x, Info.VkViewport.y},
                .extent = {Info.VkViewport.width, Info.VkViewport.height},
                }, Rect))
            {
                Range->push_back({
                    .Rect = Rect,
                    .TexCoordRect = Glyph.TexCoordRect,
                    .Tint = Text.Tint,
                    .OutlineTint = Colors::White,
                    .OutlineThickness = 0,
                    .TextureIndex = Glyph.BindlessTextureIndex,
                    .SamplerIndex = Glyph.SamplerIndex,
                    .MsdfPixelRange = Glyph.MsdfPixelRange,
                    });
            }
        }
    }

    if (!Range->empty())
    {
        constexpr auto VisualSharedBinding{0uz};
        constexpr auto InstanceBufferBinding{1uz};

        auto& DsInstance{this->TextMaterialInstance->Vk_GetUniqueDescriptorSet(0, Info.Frame)};
        check(DsInstance.Resources.size() == 2)

        LVec2F Dimensions{Info.VkViewport.width, Info.VkViewport.height};
        auto VisualSharedWriteInfo{UBO::VisualShared{
                .Proj = glm::orthoRH_ZO(0.0f, Dimensions.x, 0.0f, Dimensions.y, 0.0f, 1.0f),
                .Gamma = *GetSingleton<JUserPreferences>().InterfaceGamma,
                }
            .upload(DsInstance.Resources[VisualSharedBinding].AsBuffer())
            .write_info(*DsInstance.Resources[VisualSharedBinding].AsBuffer())
            };

        auto InstanceBufferWriteInfo{Range.upload(DsInstance.Resources[InstanceBufferBinding].AsBuffer())};

        std::array Writes{
            vk::WriteDescriptorSet{
                .dstSet = *DsInstance,
                .dstBinding = VisualSharedBinding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = UBO::VisualShared::descriptor_type(),
                .pBufferInfo = &VisualSharedWriteInfo,
                },
            vk::WriteDescriptorSet{
                .dstSet = *DsInstance,
                .dstBinding = InstanceBufferBinding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = SSBO::VisualInstance::descriptor_type(),
                .pBufferInfo = &InstanceBufferWriteInfo
                },
            };
        Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});

        std::array<vk::DescriptorSet, 2> DescriptorSetsToBind;
        DescriptorSetsToBind[0] = *DsInstance;
        DescriptorSetsToBind[1] = *Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetBindlessTextureArrayDescriptorSet();
        Info.CommandBuffer.bindDescriptorSets2({
            .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            .layout = *this->TextMaterialInstance->Material->Pipeline.pipeline_layout,
            .firstSet = 0,
            .descriptorSetCount = DescriptorSetsToBind.size(),
            .pDescriptorSets = DescriptorSetsToBind.data(),
            .dynamicOffsetCount = 0,
            .pDynamicOffsets = nullptr
            });

        Info.CommandBuffer.setPolygonModeEXT(vk::PolygonMode::eFill);
        Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *this->TextMaterialInstance->Material->Pipeline);
        Info.CommandBuffer.draw(4, static_cast<u32>(Range->size()), 0, 0);
    }
}
