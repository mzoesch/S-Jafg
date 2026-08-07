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
    #define TINYGLTF3_ENABLE_FS
    #define TINYGLTF3_IMPLEMENTATION
    #include <tiny_gltf_v3.h>
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

Jafg::LStaticMesh::EResult LoadViaTinyGltf(Jafg::LStaticMesh& Target)
{
    typedef Jafg::LStaticMesh::Vertex LVertex;

    tg3_parse_options Options{};
    Options.required_sections = TG3_REQUIRE_VERSION;
    Options.strictness = TG3_PERMISSIVE;
    Options.memory.memory_budget = TINYGLTF3_MAX_MEMORY_BYTES;
    Options.memory.arena_block_size = TG3__ARENA_DEFAULT_BLOCK_SIZE;
    Options.max_external_file_size = 0;
#if JAFG_IN_SHIPPING
    Options.validate_indices = 0;
#else /* JAFG_IN_SHIPPING */
    Options.validate_indices = 1;
#endif /* !JAFG_IN_SHIPPING */

    tg3_error_stack ErrorStack{};
    ::tg3_error_stack_init(&ErrorStack);

    tg3_model Model{};

    algo::raii_leave{[&]
    {
        ::tg3_model_free(&Model);
        ::tg3_error_stack_free(&ErrorStack);
    }};

#if !JAFG_PLATFORM_USES_UTF8
    auto NativeString{Target.GetPath().string()};
#endif /* !JAFG_PLATFORM_USES_UTF8 */
    if (auto Error{
#if JAFG_PLATFORM_USES_UTF8
        ::tg3_parse_file(&Model, &ErrorStack, Target.GetPath().native().c_str(), static_cast<uint32_t>(Target.GetPath().native().size()), &Options)
#else /* JAFG_PLATFORM_USES_UTF8 */
        ::tg3_parse_file(&Model, &ErrorStack, NativeString.c_str(), static_cast<uint32_t>(NativeString.size()), &Options)
#endif /* !JAFG_PLATFORM_USES_UTF8 */
        }; Error != TG3_OK)
    {
        for (decltype(tg3_error_stack::count) Idx{0uz}; Idx < ErrorStack.count; ++Idx)
        {
            switch (ErrorStack.entries[Idx].severity)
            {
            case TG3_SEVERITY_INFO:
            {
                LOG_INFO(LogRhi, "[{}]: {}"
                    , Target.GetPath()
                    , ErrorStack.entries[Idx].message ? ErrorStack.entries[Idx].message : "(null)"
                    )
                break;
            }
            case TG3_SEVERITY_WARNING:
            {
                LOG_WARNING(LogRhi, "[{}]: {}"
                    , Target.GetPath()
                    , ErrorStack.entries[Idx].message ? ErrorStack.entries[Idx].message : "(null)"
                    )
                break;
            }
            case TG3_SEVERITY_ERROR:
            {
                LOG_ERROR(LogRhi, "[{}]: {}"
                    , Target.GetPath()
                    , ErrorStack.entries[Idx].message ? ErrorStack.entries[Idx].message : "(null)"
                    )
                break;
            }
            }
        }

        if (Error == TG3_ERR_FILE_NOT_FOUND)
        {
            return Jafg::LStaticMesh::EResult::FileNotFound;
        }
        return Jafg::LStaticMesh::EResult::LoadingError;
    }

    {
        auto& Asset{Model.asset};
        LOG_TRACE(LogRhi, "[{}]: Asset: v[{}>={}] g[{}]",
            Target.GetPath()
            , LStringView{Asset.version.data, Asset.version.len}
            , LStringView{Asset.min_version.data, Asset.min_version.len}
            , LStringView{Asset.generator.data, Asset.generator.len}
            )
    }

    LOG_TRACE(LogRhi, "[{}]: {} meshes, {} materials, {} textures, {} samplers, {} animations, {} skins, {} nodes"
                        ", {} buffers, {} buffer views, {} accessors."
        , Target.GetPath(), Model.meshes_count, Model.materials_count, Model.textures_count, Model.samplers_count
        , Model.animations_count, Model.skins_count, Model.nodes_count, Model.buffers_count, Model.buffer_views_count, Model.accessors_count
        )

    auto FindAttribute{[](tg3_str_int_pair const* Attributes, decltype(tg3_primitive::attributes_count) Count, char const* Key)
        -> std::optional<decltype(tg3_primitive::attributes_count)>
    {
        check(Count == 0 || Attributes)
        for (decltype(Count) Idx{0uz}; Idx < Count; ++Idx)
        {
            if (tg3_str_equals_cstr(Attributes[Idx].key, Key))
            {
                return Idx;
            }
        }
        return std::nullopt;
    }};

    std::unordered_map<Jafg::LStaticMesh::Vertex, u32> uniqueVertices;
    for (decltype(Model.meshes_count) MeshIdx{0uz}; MeshIdx < Model.meshes_count; ++MeshIdx)
    {
        tg3_mesh const& Mesh{Model.meshes[MeshIdx]};
        for (decltype(Mesh.primitives_count) PrimitiveIdx{0uz}; PrimitiveIdx < Mesh.primitives_count; ++PrimitiveIdx)
        {
            tg3_primitive const& Primitive{Mesh.primitives[PrimitiveIdx]};

            tg3_accessor const& IdxAccessor{Model.accessors[Primitive.indices]};
            tg3_buffer_view const& IdxBufferView{Model.buffer_views[IdxAccessor.buffer_view]};
            tg3_buffer const& IdxBuffer{Model.buffers[IdxBufferView.buffer]};

            auto PositionAttribute{FindAttribute(Primitive.attributes, Primitive.attributes_count, "POSITION")};
            if (!PositionAttribute)
            {
                LOG_FATAL(LogRhi, "[{}]: Primitive [{}] has no POSITION attribute.", Target.GetPath(), Primitive.indices)
            }
            tg3_accessor const& PosAccessor{Model.accessors[*PositionAttribute]};
            tg3_buffer_view const& PosBufferView{Model.buffer_views[PosAccessor.buffer_view]};
            tg3_buffer const& PosBuffer{Model.buffers[PosBufferView.buffer]};

            tg3_accessor const* NormalAccessor{};
            tg3_buffer_view const* NormalBufferView{};
            tg3_buffer const* NormalBuffer{};
            if (auto NormalAttribute{FindAttribute(Primitive.attributes, Primitive.attributes_count, "NORMAL")}; NormalAttribute)
            {
                NormalAccessor   = &Model.accessors[*NormalAttribute];
                NormalBufferView = &Model.buffer_views[NormalAccessor->buffer_view];
                NormalBuffer     = &Model.buffers[NormalBufferView->buffer];
            }
            else
            {
                LOG_WARNING(LogRhi, "[{}]: Primitive [{}] has no NORMAL attribute.", Target.GetPath(), Primitive.indices)
            }

            tg3_accessor const* TexCoordAccessor{};
            tg3_buffer_view const* TexCoordBufferView{};
            tg3_buffer const* TexCoordBuffer{};
            if (auto TexCoordAttribute{FindAttribute(Primitive.attributes, Primitive.attributes_count, "TEXCOORD_0")}; TexCoordAttribute)
            {
                TexCoordAccessor   = &Model.accessors[*TexCoordAttribute];
                TexCoordBufferView = &Model.buffer_views[TexCoordAccessor->buffer_view];
                TexCoordBuffer     = &Model.buffers[TexCoordBufferView->buffer];
            }
            else
            {
                LOG_WARNING(LogRhi, "[{}]: Primitive [{}] has no TEXCOORD_0 attribute.", Target.GetPath(), Primitive.indices)
            }

            tg3_accessor const* TangentAccessor{};
            tg3_buffer_view const* TangentBufferView{};
            tg3_buffer const* TangentBuffer{};
            if (auto TangentAttribute{FindAttribute(Primitive.attributes, Primitive.attributes_count, "TANGENT")}; TangentAttribute)
            {
                TangentAccessor   = &Model.accessors[*TangentAttribute];
                TangentBufferView = &Model.buffer_views[TangentAccessor->buffer_view];
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

                auto* pPosition{reinterpret_cast<f32 const*>(&PosBuffer.data.data[
                    PosBufferView.byte_offset + PosAccessor.byte_offset + Idx * sizeof(decltype(Vertex.Position))
                    ])};
                Vertex.Position = {pPosition[0], pPosition[1], pPosition[2]};

                if (NormalAccessor)
                {
                    check(NormalBufferView && NormalBuffer)
                    auto* pNormal{reinterpret_cast<f32 const*>(&NormalBuffer->data.data[
                        NormalBufferView->byte_offset + NormalAccessor->byte_offset + Idx * sizeof(decltype(Vertex.Normal))
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
                    auto* pTexCoord{reinterpret_cast<f32 const*>(&TexCoordBuffer->data.data[
                        TexCoordBufferView->byte_offset + TexCoordAccessor->byte_offset + Idx * sizeof(decltype(Vertex.TexCoord))
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
                    auto* pTangent{reinterpret_cast<f32 const*>(&TangentBuffer->data.data[
                        TangentBufferView->byte_offset + TangentAccessor->byte_offset + Idx * sizeof(decltype(Vertex.Tangent))
                        ])};
                    Vertex.Tangent = {pTangent[0], pTangent[1], pTangent[2], pTangent[3]};
                }
                else
                {
                    Vertex.Tangent = {0.0f, 0.0f, 0.0f, 1.0f};
                }

                Target.Vertices.emplace_back(std::move(Vertex));
            }

            auto const* pIdxData{&IdxBuffer.data.data[IdxBufferView.byte_offset + IdxAccessor.byte_offset]};
            auto IdxCount{IdxAccessor.count};
            auto IdxStride{tg3_component_size(IdxAccessor.component_type)};
            if (IdxStride < static_cast<decltype(IdxStride)>(1))
            {
                LOG_FATAL(LogRhi, "[{}]: Could not infer index stride from component type [{}].",
                    Target.GetPath(), IdxAccessor.component_type
                    )
            }

            Target.Indices.reserve(Target.Indices.size() + IdxCount);
            for (auto Idx{0uz}; Idx < IdxCount; ++Idx)
            {
                if (IdxAccessor.component_type == TG3_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    Target.Indices.emplace_back(baseVertex + *reinterpret_cast<u16 const*>(pIdxData + Idx * IdxStride));
                }
                else if (IdxAccessor.component_type == TG3_COMPONENT_TYPE_UNSIGNED_INT)
                {
                    Target.Indices.emplace_back(baseVertex + *reinterpret_cast<u32 const*>(pIdxData + Idx * IdxStride));
                }
                else if (IdxAccessor.component_type == TG3_COMPONENT_TYPE_UNSIGNED_BYTE)
                {
                    Target.Indices.emplace_back(baseVertex + *reinterpret_cast<u8 const*>(pIdxData + Idx * IdxStride));
                }
                else
                {
                    LOG_FATAL(LogRhi, "[{}]: Unsupported index component type [{}].", Target.GetPath(), IdxAccessor.component_type)
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

    if (this->Path.native().ends_with(LITERAL_TEXT(".glb")) || this->Path.native().ends_with(LITERAL_TEXT(".gltf")))
    {
        if (auto Rc{::LoadViaTinyGltf(*this)}; Rc != EResult::Success)
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
