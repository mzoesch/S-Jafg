// Copyright mzoesch. All rights reserved.

#include "Rhi/StaticMesh.h"
#include "Engine/Engine.h"
#include "Rhi/RenderInfo.h"
#include "Framework/Editor.h"
#include "Widgets/EditorFactory.h"
#include "User/UserPreferences.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/ShaderSubsystem.h"

#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-W#warnings"
    #pragma clang diagnostic ignored "-Wc++98-compat-extra-semi"
    #pragma clang diagnostic ignored "-Wcast-qual"
    #pragma clang diagnostic ignored "-Wdeprecated-literal-operator"
    #pragma clang diagnostic ignored "-Wextra-semi-stmt"
    #pragma clang diagnostic ignored "-Wformat-nonliteral"
    #pragma clang diagnostic ignored "-Wmissing-format-attribute"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wtautological-type-limit-compare"
    #pragma clang diagnostic ignored "-Wunused-function"
#endif /* JAFG_WITH_CLANG */
#if JAFG_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-function"
#endif /* JAFG_WITH_GCC */
    #define TINYOBJLOADER_IMPLEMENTATION
    #include <tiny_obj_loader.h>
    #define TINYGLTF_IMPLEMENTATION
    #include <tiny_gltf.h>
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */
#if JAFG_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* JAFG_WITH_GCC */

static_assert(sizeof(uint8_t) == sizeof(u8));
static_assert(sizeof(uint16_t) == sizeof(u16));
static_assert(sizeof(uint32_t) == sizeof(u32));

namespace
{

Jafg::LVertexInputRegistrator<Jafg::LStaticMesh::Vertex> _{};

enum struct ETinyObjHint
{
    Binary,
    Json,
};

Jafg::LStaticMesh::EResult LoadViaTinyGltf(Jafg::LStaticMesh& Target, ETinyObjHint Hint)
{
    typedef Jafg::LStaticMesh::Vertex LVertex;

    tinygltf::TinyGLTF Ldr;
    tinygltf::Model Model;
    LString Warning;
    LString Error;

    bool Result{};
    if (Hint == ETinyObjHint::Binary)
    {
        LOG_TRACE(LogRhi, "Loading static mesh [{}] via tinygltf as binary glTF.", Target.GetPath())
#if JAFG_PLATFORM_USES_UTF8
        Result = Ldr.LoadBinaryFromFile(&Model, &Warning, &Error, Target.GetPath().native().c_str());
#else /* JAFG_PLATFORM_USES_UTF8 */
        Result = Ldr.LoadBinaryFromFile(&Model, &Error, &Warning, Target.GetPath().string().c_str());
#endif /* !JAFG_PLATFORM_USES_UTF8 */
    }
    else
    {
        check(Hint == ETinyObjHint::Json)
#if JAFG_PLATFORM_USES_UTF8
        Result = Ldr.LoadASCIIFromFile(&Model, &Warning, &Error, Target.GetPath().native().c_str());
#else /* JAFG_PLATFORM_USES_UTF8 */
        Result = Ldr.LoadASCIIFromFile(&Model, &Error, &Warning, Target.GetPath().string().c_str());
#endif /* !JAFG_PLATFORM_USES_UTF8 */
    }
    if (Warning.empty() == false) { LOG_WARNING(LogRhi, "tinygltf: {}", Warning) }
    if (Error.empty() == false) { LOG_ERROR(LogRhi, "tinygltf: {}", Error) }
    if (Result == false)
    {
        return Jafg::LStaticMesh::EResult::LoadingError;
    }

    {
        auto& Asset{Model.asset};
        LOG_TRACE(LogRhi, "[{}]: Asset: v[{}>={}] g[{}]",
            Target.GetPath(), Asset.version, Asset.minVersion, Asset.generator
            )
    }

    LOG_TRACE(LogRhi, "[{}]: {} meshes, {} materials, {} textures, {} samplers, {} animations, {} skins, {} nodes"
                        ", {} buffers, {} buffer views, {} accessors."
        , Target.GetPath(), Model.meshes.size(), Model.materials.size(), Model.textures.size(), Model.samplers.size()
        , Model.animations.size(), Model.skins.size(), Model.nodes.size(), Model.buffers.size(), Model.bufferViews.size(), Model.accessors.size()
        )

    std::unordered_map<Jafg::LStaticMesh::Vertex, u32> uniqueVertices;
    for (const auto &Mesh : Model.meshes)
    {
        for (const  auto &Primitive : Mesh.primitives)
        {
            tinygltf::Accessor const& IdxAccessor{Model.accessors[Primitive.indices]};
            tinygltf::BufferView const& IdxBufferView{Model.bufferViews[IdxAccessor.bufferView]};
            tinygltf::Buffer const& IdxBuffer{Model.buffers[IdxBufferView.buffer]};

            if (Primitive.attributes.contains("POSITION") == false)
            {
                LOG_FATAL(LogRhi, "[{}]: Primitive [{}] has no POSITION attribute.", Target.GetPath(), Primitive.indices)
            }
            tinygltf::Accessor const& PosAccessor{Model.accessors[Primitive.attributes.at("POSITION")]};
            tinygltf::BufferView const& PosBufferView{Model.bufferViews[PosAccessor.bufferView]};
            tinygltf::Buffer const& PosBuffer{Model.buffers[PosBufferView.buffer]};

            tinygltf::Accessor const* NormalAccessor{};
            tinygltf::BufferView const* NormalBufferView{};
            tinygltf::Buffer const* NormalBuffer{};
            if (auto It{Primitive.attributes.find("NORMAL")}; It != Primitive.attributes.end())
            {
                NormalAccessor   = &Model.accessors[It->second];
                NormalBufferView = &Model.bufferViews[NormalAccessor->bufferView];
                NormalBuffer     = &Model.buffers[NormalBufferView->buffer];
            }
            else
            {
                LOG_WARNING(LogRhi, "[{}]: Primitive [{}] has no NORMAL attribute.", Target.GetPath(), Primitive.indices)
            }

            tinygltf::Accessor const* TexCoordAccessor{};
            tinygltf::BufferView const* TexCoordBufferView{};
            tinygltf::Buffer const* TexCoordBuffer{};
            if (auto It{Primitive.attributes.find("TEXCOORD_0")}; It != Primitive.attributes.end())
            {
                TexCoordAccessor   = &Model.accessors[It->second];
                TexCoordBufferView = &Model.bufferViews[TexCoordAccessor->bufferView];
                TexCoordBuffer     = &Model.buffers[TexCoordBufferView->buffer];
            }
            else
            {
                LOG_WARNING(LogRhi, "[{}]: Primitive [{}] has no TEXCOORD_0 attribute.", Target.GetPath(), Primitive.indices)
            }

            tinygltf::Accessor const* TangentAccessor{};
            tinygltf::BufferView const* TangentBufferView{};
            tinygltf::Buffer const* TangentBuffer{};
            if (auto It{Primitive.attributes.find("TANGENT")}; It != Primitive.attributes.end())
            {
                TangentAccessor   = &Model.accessors[It->second];
                TangentBufferView = &Model.bufferViews[TangentAccessor->bufferView];
                TangentBuffer     = &Model.buffers[TangentBufferView->buffer];
            }
            else
            {
                LOG_WARNING(LogRhi, "[{}]: Primitive [{}] has no TANGENT attribute.", Target.GetPath(), Primitive.indices)
            }

            uint32_t baseVertex = static_cast<uint32_t>(Target.Vertices.size());

            for (auto Idx{0uz}; Idx < PosAccessor.count; ++Idx)
            {
                LVertex Vertex{};

                auto* pPosition{reinterpret_cast<f32 const*>(&PosBuffer.data[
                    PosBufferView.byteOffset + PosAccessor.byteOffset + Idx * sizeof(decltype(Vertex.Position))
                    ])};
                Vertex.Position = {pPosition[0], pPosition[1], pPosition[2]};

                if (NormalAccessor)
                {
                    check(NormalBufferView && NormalBuffer)
                    auto* pNormal{reinterpret_cast<f32 const*>(&NormalBuffer->data[
                        NormalBufferView->byteOffset + NormalAccessor->byteOffset + Idx * sizeof(decltype(Vertex.Normal))
                        ])};
                    Vertex.Normal = {pNormal[0], pNormal[1], pNormal[2]};
                }
                else
                {
                    Vertex.Normal = {0.0f, 0.0f, 0.0f};
                }

                if (TexCoordAccessor)
                {
                    check(TexCoordBufferView && TexCoordBuffer)
                    auto* pTexCoord{reinterpret_cast<f32 const*>(&TexCoordBuffer->data[
                        TexCoordBufferView->byteOffset + TexCoordAccessor->byteOffset + Idx * sizeof(decltype(Vertex.TexCoord))
                        ])};
                    Vertex.TexCoord = {pTexCoord[0], pTexCoord[1]};
                }
                else
                {
                    Vertex.TexCoord = {0.0f, 0.0f};
                }

                if (TangentAccessor)
                {
                    check(TangentBufferView && TangentBuffer)
                    auto* pTangent{reinterpret_cast<f32 const*>(&TangentBuffer->data[
                        TangentBufferView->byteOffset + TangentAccessor->byteOffset + Idx * sizeof(decltype(Vertex.Tangent))
                        ])};
                    Vertex.Tangent = {pTangent[0], pTangent[1], pTangent[2], pTangent[3]};
                }
                else
                {
                    Vertex.Tangent = {0.0f, 0.0f, 0.0f, 1.0f};
                }

                Target.Vertices.emplace_back(std::move(Vertex));
            }

            auto const* pIdxData{&IdxBuffer.data[IdxBufferView.byteOffset + IdxAccessor.byteOffset]};
            auto IdxCount{IdxAccessor.count};
            auto IdxStride{
                IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT   ? sizeof(u32) :
                IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? sizeof(u16) :
                IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE  ? sizeof(u8) :
                0uz
                };
            if (IdxStride == 0uz)
            {
                LOG_FATAL(LogRhi, "[{}]: Could not infer index stride from component type [{}].",
                    Target.GetPath(), IdxAccessor.componentType
                    )
            }

            Target.Indices.reserve(Target.Indices.size() + IdxCount);
            for (auto Idx{0uz}; Idx < IdxCount; ++Idx)
            {
                if (IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    Target.Indices.emplace_back(baseVertex + *reinterpret_cast<u16 const*>(pIdxData + Idx * IdxStride));
                }
                else if (IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                {
                    Target.Indices.emplace_back(baseVertex + *reinterpret_cast<u32 const*>(pIdxData + Idx * IdxStride));
                }
                else if (IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                {
                    Target.Indices.emplace_back(baseVertex + *reinterpret_cast<u8 const*>(pIdxData + Idx * IdxStride));
                }
                else
                {
                    std::unreachable();
                }
            }
        }
    }

    return Jafg::LStaticMesh::EResult::Success;
}

} /* ~Namespace <Anonymous> */

template<>
Jafg::Detail::LNodeFactoryBase Jafg::GetEditorNode<Jafg::LStaticMeshRef>(TEditorNodeCreateInfo<LStaticMeshRef> const& Info) noexcept
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    auto SharedState(std::make_shared<LEditorRowState>());

    return NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
    [
        EditorComponentLabel(Info.Viewport, "Mesh", SharedState)
        + EditorComponentContentWrapper(Info.Viewport, SharedState)
        [
            NewNode(Info.Viewport).Class<WHParent>()
                .Anchor(EAnchor::Fill)
            [
                NewNode(Info.Viewport).Class<WEditableTextButton>()
                    .Anchor(EAnchor::HFill)
                    .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
                    .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
                    .Content(Info.Field ? Info.Field->GetPath().string() : "<not-set>")
                + NewNode(Info.Viewport).Class<WTextButtonIconizedDouble>()
                    .Style(Prefs.EditorSecondaryButton<LBoxBrush>())
                    .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
                    .Content("Browse")
                    .LeftIcon("Icons/Jafg.DirectorySearch")
                    .OnKeyEventFocused([FieldPtr=&Info.Field](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton) && Self.AabbTest(Info))
                        {
                            check(FieldPtr)
                            auto& Field{*FieldPtr};
                            check(Field.get())

                            LPath DefaultPath;
                            if (!Field->GetPath().empty() && Field->GetPath().has_parent_path())
                            {
                                DefaultPath = Field->GetPath().parent_path();
                            }
                            else
                            {
                                DefaultPath = finder::content_dir();
                            }

                            if (auto AbsolutePath{Info.Viewport.GetSurface().OpenBlockingDialogForFile({
                                .Filters = {FileDialogFilterPresets::Models}
                                }, DefaultPath)}; !AbsolutePath)
                            {
                                LOG_INFO(LogEditor, "Directory selection cancelled. No path provided. Discarding request.")
                            }
                            else
                            {
                                LOG_VERBOSE(LogEditor, "Selected path: [{}].", *AbsolutePath)
                                if (!finder::descendant_of(*AbsolutePath, finder::content_dir()))
                                {
                                    LOG_FATAL(LogEditor, "[{}]: Path has to be a descendant of [{}].",
                                        *AbsolutePath, absolute(finder::content_dir()))
                                }
                                auto Path{finder::relative(*AbsolutePath, finder::current_path())};
                                EStaticMeshState NewState{EStaticMeshStateBits::None};
                                if (Field->IsOnHost())
                                {
                                    NewState |= EStaticMeshStateBits::Host;
                                }
                                if (Field->IsOnDevice())
                                {
                                    NewState |= EStaticMeshStateBits::Device;
                                }

                                Field = GEngine->GetSubsystemChecked<JMeshSubsystem>()->FromFile(Path, NewState);
                            }

                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
            ]
        ]
        + EditorResetButton(Info.Viewport, true, [Field=Info.Field](WButton& Self)
        {
            check(!!Field)
            check(GEngine)
            Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_EditorWaitIdle();
            Field->FreeFromHost();
            Field->FreeFromDevice();
            return;
        }, LEditorRowState::Lambda<WButton>(SharedState))
    ];
}

Jafg::LStaticMesh::EResult Jafg::LStaticMesh::LoadToHost()
{
    check(this->Path.empty() == false)
    LOG_VERBOSE(LogRhi, "Loading static mesh from path [{}].", this->Path)

    if (is_regular_file(this->Path) == false)
    {
        return EResult::FileNotFound;
    }

    if (this->IsOnHost())
    {
        LOG_VERBOSE(LogRhi, "Static mesh [{}] already loaded to host memory. Freeing previous host memory and reloading.", this->Path)
        this->FreeFromHost();
    }
    check(this->Vertices.empty() && this->Indices.empty())

    if (this->Path.native().ends_with(LITERAL_TEXT(".glb")))
    {
        if (auto Rc{::LoadViaTinyGltf(*this, ETinyObjHint::Binary)}; Rc != EResult::Success)
        {
            return Rc;
        }
    }
    else if (this->Path.native().ends_with(LITERAL_TEXT(".gltf")))
    {
        if (auto Rc{::LoadViaTinyGltf(*this, ETinyObjHint::Json)}; Rc != EResult::Success)
        {
            return Rc;
        }
    }
    else
    {
        LOG_FATAL(LogRhi, "Unsupported static mesh file format: [{}].", this->Path)
    }

    if (this->Vertices.empty() || this->Indices.empty())
    {
        LOG_ERROR(LogRhi, "[{}]: No vertices or indices were loaded.", this->GetPath())
        return EResult::LoadingError;
    }

    this->Aabb.min = this->Vertices[0].Position;
    this->Aabb.max = this->Vertices[0].Position;
    for (auto& V: this->Vertices)
    {
        this->Aabb.min = maths::min(this->Aabb.min, V.Position);
        this->Aabb.max = maths::max(this->Aabb.max, V.Position);
    }

    return EResult::Success;
}

void Jafg::LStaticMesh::LoadToDevice()
{
    check(this->IsOnHost())
    LOG_TRACE(LogRhi, "Loading static mesh [{}] to device.", this->Path)

    if (this->IsOnDevice())
    {
        LOG_TRACE(LogRhi, "Static mesh [{}] already loaded to device memory. Freeing previous device memory and reloading.", this->Path)
        this->FreeFromDevice();
    }
    check(this->IndexCount == 0 && this->VertexBuffer.get_allocation() == nullptr && this->IndexBuffer.get_allocation() == nullptr)

    this->VertexBuffer = Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Vertex({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(this->Vertices[0]) * this->Vertices.size()},
        .Data = this->Vertices.data(),
        }));

    this->IndexBuffer = Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Index({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(this->Indices[0]) * this->Indices.size()},
        .Data = this->Indices.data(),
        }));

    this->IndexCount = static_cast<u32>(this->Indices.size());
}

void Jafg::LStaticMesh::Render(LActorRenderInfo const& Info, LWorldTrans const& Transform, LMaterialInstance const& Instance) const
{
#if JAFG_WITH_EDITOR
    /* Ok, but only in the editor. Else we should cull this comp from rendering beforehand; because performance. */
    if (!this->IsOnDevice())
    {
        return;
    }
#endif /* !JAFG_WITH_EDITOR */
    check(this->IsOnDevice())

    check(Instance.Material.get())

    auto& Material{*Instance.Material};
    auto& Pipeline{Material.Pipeline};
    auto& MaterialTemplate{Material.Template};
    auto& Shader{MaterialTemplate.my_shader};

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Pipeline);

    {
        /* Vulkan specs states at least 4. */
        std::array<vk::DescriptorSet, rhi::bound_descriptor_set_limit> DescriptorSetsToBind;
        u32 NumDescriptorSets{0};
        for (auto It{Shader.begin_space()}; It != Shader.end_space(); ++It)
        {
            checkCode
            (
                if (*It >= 4)
                {
                    LOG_FATAL(LogRhi, "Expected at most 4 descriptor sets to bind. But got [{}].", *It)
                }
            )

            if (auto* Set{Instance.Vk_FindUniqueDescriptorSet(*It, Info.Frame)}; Set)
            {
                DescriptorSetsToBind[*It] = **Set;
            }
            else
            {
                if (auto* Attribute{It->find_user_attribute("Shared", 1uz)}; Attribute != nullptr)
                {
                    if (Attribute->front() == "Jafg::UBO::WorldData"sv)
                    {
                        check(DescriptorSetsToBind[*It] == nullptr)
                        DescriptorSetsToBind[*It] = Info.WorldDataDescriptorSet;
                    }
                    else if (auto It2{Info.AdditionalSharedDescriptorSets.find(Attribute->front())}; It2 != Info.AdditionalSharedDescriptorSets.end())
                    {
                        unimplemented()
                    }
                    else
                    {
                        LOG_FATAL(LogRhi, "[{}]: No descriptor set for space [{}@{}] found. Unexpected shared attribute [{}]."
                            , Shader.Identifier, *It, Info.Frame, Attribute->front())
                    }
                }
                else
                {
                    LOG_FATAL(LogRhi, "[{}]: No descriptor set for space [{}@{}] found."
                        , Shader.Identifier, *It, Info.Frame)
                }
            }

            NumDescriptorSets = maths::max(NumDescriptorSets, *It + 1);
        }

        checkCode
        (
            for (vk::DescriptorSet const& SetToBind: DescriptorSetsToBind | algo::views::take(NumDescriptorSets))
            {
                check(!!SetToBind)
            }
        )

        if (NumDescriptorSets > 0)
        {
            Info.CommandBuffer.bindDescriptorSets2({
                /* TODO: Is this correct? The sets are vertex && fragment respectively -- not vertex | fragment. */
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .layout = *Pipeline.pipeline_layout,
                .firstSet = 0,
                .descriptorSetCount = NumDescriptorSets,
                .pDescriptorSets = DescriptorSetsToBind.data(),
                .dynamicOffsetCount = 0,
                .pDynamicOffsets = nullptr
                });
        }
    }

    for (auto const& PushConstant: Shader.push_constant_iter())
    {
        checkCode
        (
            if (!PushConstant.CxxName.has_value())
            {
                LOG_FATAL(LogRhi, "[{}]: Push constant at [{}@{}] has no CXX name."
                    , Shader.Identifier, PushConstant.Space, PushConstant.Index)
            }
        )
        auto& Pc{Info.ShaderSubsystem.GetPushConstant(*PushConstant.CxxName)};
        check(Pc.PushForActor)
        Pc.PushForActor(Info, Material, {Transform});
    }

    this->DrawIndexed(Info);
}

void Jafg::LStaticMesh::DrawIndexed(LRenderInfo const& Info) const
{
    check(this->IndexCount > 0)
    check(*this->VertexBuffer && *this->IndexBuffer)

    Info.CommandBuffer.bindVertexBuffers(0, *this->VertexBuffer, {0});
    Info.CommandBuffer.bindIndexBuffer(*this->IndexBuffer, 0, vk::IndexTypeValue<decltype(this->Indices)::value_type>::value);

    Info.CommandBuffer.drawIndexed(static_cast<u32>(this->IndexCount), 1, 0, 0, 0);
}
