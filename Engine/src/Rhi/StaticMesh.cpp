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
    #define TINYGLTF3_IMPLEMENTATION
    #include "tiny_gltf_v3.h"
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

NODISCARD inline f32 ReadComponentAsFloat(uint8_t const* Data, i32 ComponentType, bool Normalized) noexcept
{
    switch (ComponentType)
    {
        case TG3_COMPONENT_TYPE_BYTE:
        {
            i8 Component; std::memcpy(&Component, Data, sizeof(Component));
            return Normalized
                ? (Component / 127.0f < -1.0f
                    ? -1.0f
                    : Component / 127.0f)
                : static_cast<f32>(Component);
        }
        case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:
        {
            u8 Component; std::memcpy(&Component, Data, sizeof(Component));
            return Normalized
                ? Component / 255.0f
                : static_cast<f32>(Component);
        }
        case TG3_COMPONENT_TYPE_SHORT:
        {
            i16 Component; std::memcpy(&Component, Data, sizeof(Component));
            return Normalized
                ? (Component / 32767.0f < -1.0f
                    ? -1.0f
                    : Component / 32767.0f)
                : static_cast<f32>(Component);
        }
        case TG3_COMPONENT_TYPE_UNSIGNED_SHORT:
        {
            u16 Component; std::memcpy(&Component, Data, sizeof(Component));
            return Normalized
                ? Component / 65535.0f
                : static_cast<f32>(Component);
        }
        case TG3_COMPONENT_TYPE_UNSIGNED_INT:
        {
            u32 Component; std::memcpy(&Component, Data, sizeof(Component));
            return static_cast<f32>(Component);
        }
        case TG3_COMPONENT_TYPE_FLOAT:
        {
            float Component; std::memcpy(&Component, Data, sizeof(f32));
            return Component;
        }
        default:
        {
            LOG_FATAL(LogRhi, "Unsupported gltf component type [{}].", ComponentType)
        }
    }
}

NODISCARD inline bool ReadAccessorFloats(
      tg3_model const& Model, i32 AccessorIdx
    , i32 TargetComponentCount
    , TArray<f32>* Result
    )
{
    check(Result)

    if (AccessorIdx < 0 || static_cast<decltype(Model.accessors_count)>(AccessorIdx) >= Model.accessors_count)
    {
        return false;
    }

    tg3_accessor const& Accessor{Model.accessors[AccessorIdx]};

    /* Sparse-only / bufferView-less accessors are not handled here; zero-fill instead
       of failing so that a mesh with e.g. sparse morph-only data is still allowed to loads. */
    if (Accessor.buffer_view < 0)
    {
        Result->assign(static_cast<size_t>(Accessor.count) * TargetComponentCount, 0.0f);
        return true;
    }

    tg3_buffer_view const& BufferView{Model.buffer_views[Accessor.buffer_view]};
    tg3_buffer const& Buffer{Model.buffers[BufferView.buffer]};
    if (!Buffer.data.data)
    {
        /* Unresolved external buffer. */
        return false;
    }

    auto ComponentNumber{tg3_num_components(Accessor.type)};
    auto ComponentSize{tg3_component_size(Accessor.component_type)};
    auto Stride{tg3_accessor_byte_stride(&Accessor, &BufferView)};
    if (ComponentNumber <= 0 || ComponentSize <= 0 || Stride <= 0)
    {
        return false;
    }

    uint8_t const* Base{Buffer.data.data + BufferView.byte_offset + Accessor.byte_offset};

    Result->resize(static_cast<size_t>(Accessor.count) * TargetComponentCount);
    for (decltype(Accessor.count) Idx{}; Idx < Accessor.count; ++Idx)
    {
        uint8_t const* Element{Base + Idx * static_cast<uint64_t>(Stride)};
        for (i32 ComponentIdx{}; ComponentIdx < TargetComponentCount; ++ComponentIdx)
        {
            f32 Value{};
            if (ComponentIdx < ComponentNumber)
            {
                Value = ::ReadComponentAsFloat(Element + ComponentIdx * ComponentSize, Accessor.component_type, Accessor.normalized != 0);
            }
            else if (ComponentIdx == 3)
            {
                /* Sane default alpha/w for accessors read as a LVec4F. */
                Value = 1.0f;
            }
            (*Result)[Idx * TargetComponentCount + ComponentIdx] = Value;
        }
    }
    return true;
}

NODISCARD inline bool ReadIndices(tg3_model const& Model, i32 AccessorIndex, TArray<u32>* Result)
{
    check(Result)

    if (AccessorIndex < 0 || static_cast<uint32_t>(AccessorIndex) >= Model.accessors_count)
    {
        return false;
    }

    tg3_accessor const& Accessor{Model.accessors[AccessorIndex]};
    if (Accessor.buffer_view < 0)
    {
        return false;
    }

    tg3_buffer_view const& BufferView{Model.buffer_views[Accessor.buffer_view]};
    tg3_buffer const& Buffer{Model.buffers[BufferView.buffer]};
    if (!Buffer.data.data)
    {
        return false;
    }

    auto Stride{tg3_accessor_byte_stride(&Accessor, &BufferView)};
    if (Stride <= 0)
    {
        return false;
    }

    uint8_t const* Base{Buffer.data.data + BufferView.byte_offset + Accessor.byte_offset};

    Result->resize(Accessor.count);
    for (auto Idx{0uz}; Idx < Accessor.count; ++Idx)
    {
        uint8_t const* Element{Base + Idx * static_cast<uint64_t>(Stride)};
        switch (Accessor.component_type)
        {
            case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:
            {
                u8 v; std::memcpy(&v, Element, sizeof(v));
                (*Result)[Idx] = v;
                break;
            }
            case TG3_COMPONENT_TYPE_UNSIGNED_SHORT:
            {
                u16 v; std::memcpy(&v, Element, sizeof(v));
                (*Result)[Idx] = v;
                break;
            }
            case TG3_COMPONENT_TYPE_UNSIGNED_INT:
            {
                u32 v; std::memcpy(&v, Element, sizeof(v));
                (*Result)[Idx] = v;
                break;
            }
            default:
            {
                /* The gltf spec disallows singed/float component types for indices. */
                (*Result)[Idx] = 0;
                break;
            }
        }
    }
    return true;
}

inline Jafg::LStaticMesh::EResult PullGltfMesh(LPath const& Path, TArray<Jafg::LStaticMesh::HostMesh>& Result)
{
    STAT_FUNCTION()
    check(Result.empty())

    std::ifstream F{Path, std::ios::binary|std::ios::ate};
    if (!F)
    {
        return Jafg::LStaticMesh::EResult::FailedToOpen;
    }
    auto FSize{F.tellg()};
    F.seekg(0, std::ios::beg);

    std::vector<std::byte> Data(static_cast<size_t>(FSize));
    if (FSize > 0 && !F.read(reinterpret_cast<char*>(Data.data()), FSize))
    {
        return Jafg::LStaticMesh::EResult::FailedToRead;
    }

    tg3_parse_options Options;
    tg3_parse_options_init(&Options);
    //# Skip image decoding.
    Options.images_as_is = 1;
    //# Parse only what we know.
    Options.skip_extras_values = 1;
    //# Only parse valid trees. Maybe disable in shipping?
    Options.validate_indices = 1;

    tg3_error_stack Errors;
    tg3_error_stack_init(&Errors);

    tg3_model Model{};
    Model.default_scene = -1;

    if (tg3_error_code Error{tg3_parse_glb(
          &Model, &Errors
        , reinterpret_cast<uint8_t*>(Data.data()), Data.size(),
        /* base_dir */ "", 0
        , &Options
        )};
        Error != TG3_OK || tg3_errors_has_error(&Errors))
    {
        decltype(tg3_errors_count(&Errors)) ErrorCount{tg3_errors_count(&Errors)};
        for (decltype(tg3_errors_count(&Errors)) Idx{}; Idx < ErrorCount; ++Idx)
        {
            const tg3_error_entry* Entry{tg3_errors_get(&Errors, Idx)};
            if (Entry)
            {
                switch (Entry->severity)
                {
                case TG3_SEVERITY_INFO:
                {
                    LOG_VERBOSE(LogRhi, "[gltf] [{}]: {}", std::to_underlying(Entry->code), Entry->message ? Entry->message  : "Unknown message.")
                    break;
                }
                case TG3_SEVERITY_WARNING:
                {
                    LOG_WARNING(LogRhi, "[gltf] [{}]: {}", std::to_underlying(Entry->code), Entry->message ? Entry->message : "Unknown message." )
                    break;
                }
                case TG3_SEVERITY_ERROR:
                {
                    LOG_ERROR(LogRhi, "[gltf] [{}]: {}", std::to_underlying(Entry->code), Entry->message ? Entry->message : "Unknown message." )
                    break;
                }
                }
            }
            else
            {
                LOG_TRACE(LogRhi, "[gltf] unknown error.")
            }
        }
        tg3_error_stack_free(&Errors);
        tg3_model_free(&Model);
        return Jafg::LStaticMesh::EResult::LoadingError;
    }

    Result.reserve(Model.meshes_count);

    for (decltype(Model.meshes_count) MeshIdx{}; MeshIdx < Model.meshes_count; ++MeshIdx)
    {
        tg3_mesh const& SrcMesh{Model.meshes[MeshIdx]};

        Jafg::LStaticMesh::HostMesh DstMesh;
        DstMesh.Name.assign(SrcMesh.name.data, SrcMesh.name.len);
        DstMesh.Primitives.reserve(SrcMesh.primitives_count);

        for (decltype(SrcMesh.primitives_count) PrimitiveIdx{}; PrimitiveIdx < SrcMesh.primitives_count; ++PrimitiveIdx)
        {
            tg3_primitive const& SrcPrimitive{SrcMesh.primitives[PrimitiveIdx]};

            auto PrimitiveMode{SrcPrimitive.mode < 0 ? TG3_MODE_TRIANGLES : SrcPrimitive.mode};
            if (PrimitiveMode != TG3_MODE_TRIANGLES)
            {
                LOG_WARNING(LogRhi, "[{}]: Skipping primitive [{}::{}] with unsupported mode [{}]."
                    , Path, DstMesh.Name, PrimitiveIdx, PrimitiveMode)
                continue;
            }

            static_assert(std::is_signed_v<decltype(SrcPrimitive.attributes[0].value)>);
            decltype(SrcPrimitive.attributes[0].value) PositionAttributeIdx{-1};
            decltype(SrcPrimitive.attributes[0].value) NormalAttributeIdx{-1};
            decltype(SrcPrimitive.attributes[0].value) TexCoordAttributeIdx{-1};
            decltype(SrcPrimitive.attributes[0].value) TangentAttributeIdx{-1};
            for (decltype(SrcPrimitive.attributes_count) AttributeIdx{}; AttributeIdx < SrcPrimitive.attributes_count; ++AttributeIdx)
            {
                tg3_str_int_pair const& Attribute{SrcPrimitive.attributes[AttributeIdx]};
                if (tg3_str_equals_cstr(Attribute.key, "POSITION"))
                {
                    PositionAttributeIdx  = Attribute.value;
                }
                else if (tg3_str_equals_cstr(Attribute.key, "NORMAL"))
                {
                    NormalAttributeIdx = Attribute.value;
                }
                else if (tg3_str_equals_cstr(Attribute.key, "TEXCOORD_0"))
                {
                    TexCoordAttributeIdx  = Attribute.value;
                }
                else if (tg3_str_equals_cstr(Attribute.key, "TANGENT"))
                {
                    TangentAttributeIdx  = Attribute.value;
                }
            }

            if (PositionAttributeIdx < 0)
            {
                LOG_WARNING(LogRhi, "[{}]: Skipping primitive [{}::{}] with no POSITION attribute."
                    , Path, DstMesh.Name, PrimitiveIdx)
                continue;
            }

            TArray<f32> Positions;
            TArray<f32> Normals;
            TArray<f32> TexCoords;
            TArray<f32> Tangents;
            if (!::ReadAccessorFloats(Model, PositionAttributeIdx, decltype(Jafg::LStaticMesh::Vertex::Position)::length(), &Positions))
            {
                continue;
            }

            const bool bValidNormals{NormalAttributeIdx >= 0 &&
                ::ReadAccessorFloats(Model, NormalAttributeIdx, decltype(Jafg::LStaticMesh::Vertex::Normal)::length(), &Normals)};
            const bool bValidTexCoords{TexCoordAttributeIdx >= 0 &&
                ::ReadAccessorFloats(Model, TexCoordAttributeIdx, decltype(Jafg::LStaticMesh::Vertex::TexCoord)::length(), &TexCoords)};
            const bool bValidTangents{TangentAttributeIdx >= 0 &&
                ::ReadAccessorFloats(Model, TangentAttributeIdx, decltype(Jafg::LStaticMesh::Vertex::Tangent)::length(), &Tangents)};

            const size_t VertexCount{Positions.size() / 3uz};

            Jafg::LStaticMesh::HostPrimitive Primitive;
            Primitive.Vertices.resize(VertexCount);
            for (auto Vertex{0uz}; Vertex < VertexCount; ++Vertex)
            {
                Jafg::LStaticMesh::Vertex& DstVertex{Primitive.Vertices[Vertex]};
                DstVertex.Position = LVec3F{Positions[Vertex * 3 + 0], Positions[Vertex * 3 + 1], Positions[Vertex * 3 + 2]};
                if (bValidNormals)
                {
                    DstVertex.Normal = LVec3F{Normals[Vertex * 3 + 0], Normals[Vertex * 3 + 1], Normals[Vertex * 3 + 2]};
                }
                else
                {
                    DstVertex.Normal = maths::zero_vector<LVec3F>;
                }
                if (bValidTexCoords)
                {
                    DstVertex.TexCoord = LVec2F{TexCoords[Vertex * 2 + 0], TexCoords[Vertex * 2 + 1]};
                }
                else
                {
                    DstVertex.TexCoord = maths::zero_vector<LVec2F>;
                }
                if (bValidTangents)
                {
                    DstVertex.Tangent = LVec4F{Tangents[Vertex * 4 + 0], Tangents[Vertex * 4 + 1], Tangents[Vertex * 4 + 2], Tangents[Vertex * 4 + 3]};
                }
                else
                {
                    DstVertex.Tangent = maths::zero_vector<LVec4F>;
                }
            }

            if (SrcPrimitive.indices >= 0)
            {
                if (!::ReadIndices(Model, SrcPrimitive.indices, &Primitive.Indices))
                {
                    Primitive.Indices.clear();
                }
            }
            else
            {
                /* TODO: Not optimal. Can we merge verticies maybe? */
                Primitive.Indices.resize(VertexCount);
                for (auto VertexIdx{0uz}; VertexIdx < VertexCount; ++VertexIdx)
                {
                    Primitive.Indices[VertexIdx] = static_cast<uint32_t>(VertexIdx);
                }
            }

            DstMesh.Primitives.push_back(std::move(Primitive));
        }

        Result.push_back(std::move(DstMesh));
    }

    tg3_error_stack_free(&Errors);
    tg3_model_free(&Model);

    Result.shrink_to_fit();

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
                    .Palette(*Prefs.InputPaletteSolid)
                    .Content(Info.Field ? Info.Field->GetPath().string() : "<not-set>")
                + NewNode(Info.Viewport).Class<WTextButtonIconizedDouble>()
                    .Palette(*Prefs.SecondaryPaletteSolid)
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
    STAT_FUNCTION()

    check(!this->Path.empty())
    LOG_VERBOSE(LogRhi, "[{}]: Loading static mesh.", this->Path)

    if (!is_regular_file(this->Path))
    {
        return EResult::FileNotFound;
    }

    if (this->IsOnHost())
    {
        LOG_VERBOSE(LogRhi, "[{}]: Already loaded to host. Reloading.", this->Path)
        this->HostMeshes.clear();
    }
    check(this->HostMeshes.empty())

    if (auto Result{::PullGltfMesh(this->Path, this->HostMeshes)}; Result != EResult::Success)
    {
        return Result;
    }
    check(this->IsOnHost())
    if constexpr (LogRhi.CompilesFor<ELogVerbosity::Trace>)
    {
        std::stringstream SS;
        SS << "Meshes: " << this->HostMeshes.size() << "\n";
        for (auto const& Mesh: this->HostMeshes)
        {
            SS << "  Mesh: " << Mesh.Name << ", Primitives: " << Mesh.Primitives.size() << "\n";
            for (auto const& Primitive: Mesh.Primitives)
            {
                SS << "    Primitive: Vertices: " << Primitive.Vertices.size() << ", Indices: " << Primitive.Indices.size() << "\n";
            }
        }
        LOG_TRACE(LogRhi, "[{}]: {}", this->Path, SS.str())
    }

    STAT_NAMED_ZONE(__CalculatingAabb, "LoadToHost::CalculatingAabb")
    this->Aabb.min = this->HostMeshes[0].Primitives[0].Vertices[0].Position;
    this->Aabb.max = this->HostMeshes[0].Primitives[0].Vertices[0].Position;
    for (auto& Mesh: this->HostMeshes)
    {
        for (auto const& Primitive: Mesh.Primitives)
        {
            for (auto const& Vertex: Primitive.Vertices)
            {
                this->Aabb.min = maths::min(this->Aabb.min, Vertex.Position);
                this->Aabb.max = maths::max(this->Aabb.max, Vertex.Position);
            }
        }
    }

    return EResult::Success;
}

void Jafg::LStaticMesh::LoadToDevice()
{
    LOG_TRACE(LogRhi, "[{}]: Loading to device.", this->Path)
    check(this->IsOnHost())

    if (this->IsOnDevice())
    {
        LOG_TRACE(LogRhi, "[{}]: Already loaded to device. Reloading.", this->Path)
        this->FreeFromDevice();
    }
    check(this->DeviceMeshes.empty())

    for (auto const& HostMesh: this->HostMeshes)
    {
        auto& DeviceMesh{this->DeviceMeshes.emplace_back(HostMesh.Name)};
        for (auto const& HostPrimitive: HostMesh.Primitives)
        {
            auto& DevicePrimitive{DeviceMesh.Primitives.emplace_back()};
            DevicePrimitive.Vertices = Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Vertex({
                .BufferCopy = vk::BufferCopy{0, 0, sizeof(decltype(HostPrimitive.Vertices)::value_type) * HostPrimitive.Vertices.size()},
                .Data = HostPrimitive.Vertices.data(),
                }));
            DevicePrimitive.Indices = Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Index({
                .BufferCopy = vk::BufferCopy{0, 0, sizeof(decltype(HostPrimitive.Indices)::value_type) * HostPrimitive.Indices.size()},
                .Data = HostPrimitive.Indices.data(),
                }));
            DevicePrimitive.IndexCount = static_cast<u32>(HostPrimitive.Indices.size());
        }
    }
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
    check(this->IsOnDevice())

    for (auto& Mesh: this->DeviceMeshes)
    {
        for (auto& Primitive: Mesh.Primitives)
        {
            check(Primitive.IndexCount > 0)
            check(*Primitive.Vertices && *Primitive.Indices)

            Info.CommandBuffer.bindVertexBuffers(0, *Primitive.Vertices, {0});
            Info.CommandBuffer.bindIndexBuffer(*Primitive.Indices, 0, vk::IndexTypeValue<decltype(this->HostMeshes[0].Primitives[0].Indices)::value_type>::value);

            Info.CommandBuffer.drawIndexed(Primitive.IndexCount, 1, 0, 0, 0);
        }
    }
}
