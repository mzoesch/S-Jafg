// Copyright mzoesch. All rights reserved.

#define TINYGLTF3_IMPLEMENTATION
#include "Gltf.h"

static_assert(sizeof(uint8_t) == sizeof(u8));
static_assert(sizeof(uint16_t) == sizeof(u16));
static_assert(sizeof(uint32_t) == sizeof(u32));

namespace
{

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

void PullMesh(tg3_model const& Model, LPath const& Path, u32 MeshIdx, Jafg::LStaticMesh::HostMesh& DstMesh)
{
    check(MeshIdx < Model.meshes_count)
    tg3_mesh const& SrcMesh{Model.meshes[MeshIdx]};

    DstMesh.Name = Jafg::Gltf::ToString(SrcMesh.name);
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
            LOG_WARNING(LogRhi, "[{}]: Skipping primitive [{}::{}] with unreadable POSITION attribute."
                , Path, DstMesh.Name, PrimitiveIdx)
            continue;
        }

        const bool bValidNormals{NormalAttributeIdx >= 0 &&
            ::ReadAccessorFloats(Model, NormalAttributeIdx, decltype(Jafg::LStaticMesh::Vertex::Normal)::length(), &Normals)};
        const bool bValidTexCoords{TexCoordAttributeIdx >= 0 &&
            ::ReadAccessorFloats(Model, TexCoordAttributeIdx, decltype(Jafg::LStaticMesh::Vertex::TexCoord)::length(), &TexCoords)};
        const bool bValidTangents{TangentAttributeIdx >= 0 &&
            ::ReadAccessorFloats(Model, TangentAttributeIdx, decltype(Jafg::LStaticMesh::Vertex::Tangent)::length(), &Tangents)};

        const size_t VertexCount{Positions.size() / 3uz};
        if (VertexCount == 0)
        {
            LOG_WARNING(LogRhi, "[{}]: Skipping primitive [{}::{}] without vertices."
                , Path, DstMesh.Name, PrimitiveIdx)
            continue;
        }

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

        if (Primitive.Indices.empty())
        {
            /* Would trip the renderer, which draws indexed only. */
            LOG_WARNING(LogRhi, "[{}]: Skipping primitive [{}::{}] without (readable) indices."
                , Path, DstMesh.Name, PrimitiveIdx)
            continue;
        }

        DstMesh.Primitives.push_back(std::move(Primitive));
    }
}

void LogErrors(LPath const& Path, tg3_error_stack const& Errors)
{
    auto const ErrorCount{tg3_errors_count(&Errors)};
    for (std::remove_const_t<decltype(ErrorCount)> Idx{}; Idx < ErrorCount; ++Idx)
    {
        tg3_error_entry const* Entry{tg3_errors_get(&Errors, Idx)};
        if (!Entry)
        {
            LOG_TRACE(LogRhi, "[{}]: [gltf] unknown error.", Path)
            continue;
        }

        switch (Entry->severity)
        {
            case TG3_SEVERITY_INFO:
            {
                LOG_VERBOSE(LogRhi, "[{}]: [gltf] [{}]: {}", Path, std::to_underlying(Entry->code), Entry->message ? Entry->message : "Unknown message.")
                break;
            }
            case TG3_SEVERITY_WARNING:
            {
                LOG_WARNING(LogRhi, "[{}]: [gltf] [{}]: {}", Path, std::to_underlying(Entry->code), Entry->message ? Entry->message : "Unknown message.")
                break;
            }
            case TG3_SEVERITY_ERROR:
            {
                LOG_ERROR(LogRhi, "[{}]: [gltf] [{}]: {}", Path, std::to_underlying(Entry->code), Entry->message ? Entry->message : "Unknown message.")
                break;
            }
        }
    }
}

//# Percent-decodes an URI reference (e.g., "My%20Ship.bin" -> "My Ship.bin").
NODISCARD LString DecodeUri(LStringView Uri)
{
    auto const Hex{[](char C) -> int
    {
        if (C >= '0' && C <= '9') { return C - '0'; }
        if (C >= 'a' && C <= 'f') { return C - 'a' + 10; }
        if (C >= 'A' && C <= 'F') { return C - 'A' + 10; }
        return -1;
    }};

    LString Result;
    Result.reserve(Uri.size());
    for (auto Idx{0uz}; Idx < Uri.size(); ++Idx)
    {
        if (Uri[Idx] == '%' && Idx + 2 < Uri.size())
        {
            if (int const Hi{Hex(Uri[Idx + 1])}, Lo{Hex(Uri[Idx + 2])}; Hi >= 0 && Lo >= 0)
            {
                Result.push_back(static_cast<char>(Hi * 16 + Lo));
                Idx += 2;
                continue;
            }
        }
        Result.push_back(Uri[Idx]);
    }
    return Result;
}

//#
//# File system callback for the external files (buffers) of a glTF file. The user data is the directory of the glTF
//# file. The URIs must stay inside that directory.
//#
int32_t ReadExternalFile(uint8_t** OutData, uint64_t* OutSize, char const* Uri, uint32_t UriLen, void* UserData)
{
    check(OutData && OutSize && UserData)
    LPath const& Directory{*static_cast<LPath const*>(UserData)};

    LPath const Relative{LPath{::DecodeUri(LStringView{Uri, UriLen})}.lexically_normal()};
    if (Relative.empty() || Relative.has_root_path() || *Relative.begin() == "..")
    {
        LOG_ERROR(LogRhi, "[{}]: External file [{}] leaves the directory of the glTF file. Refusing to read it."
            , Directory, LStringView{Uri, UriLen})
        return 0;
    }

    LPath const Path{Directory / Relative};
    std::ifstream F{Path, std::ios::binary|std::ios::ate};
    if (!F)
    {
        LOG_ERROR(LogRhi, "[{}]: Failed to open external file.", Path)
        return 0;
    }
    auto const Size{F.tellg()};
    if (Size < 0)
    {
        return 0;
    }
    F.seekg(0, std::ios::beg);

    auto* Data{new (std::nothrow) uint8_t[Size > 0 ? static_cast<size_t>(Size) : 1uz]};
    if (!Data)
    {
        return 0;
    }
    if (Size > 0 && !F.read(reinterpret_cast<char*>(Data), Size))
    {
        delete[] Data;
        LOG_ERROR(LogRhi, "[{}]: Failed to read external file.", Path)
        return 0;
    }

    *OutData = Data;
    *OutSize = static_cast<uint64_t>(Size);
    return 1;
}

void FreeExternalFile(uint8_t* Data, uint64_t /* Size */, void* /* UserData */)
{
    delete[] Data;
}

} /* ~Namespace <Anonymous> */

Jafg::LStaticMesh::EResult Jafg::Gltf::Parse(LPath const& Path, LModel& Out)
{
    STAT_FUNCTION()
    check(!Out.Native.arena_ && "Model was already parsed.")

    std::ifstream F{Path, std::ios::binary|std::ios::ate};
    if (!F)
    {
        return LStaticMesh::EResult::FailedToOpen;
    }
    auto FSize{F.tellg()};
    F.seekg(0, std::ios::beg);

    std::vector<std::byte> Data(static_cast<size_t>(FSize));
    if (FSize > 0 && !F.read(reinterpret_cast<char*>(Data.data()), FSize))
    {
        return LStaticMesh::EResult::FailedToRead;
    }

    LPath const Directory{Path.parent_path()};

    tg3_parse_options Options;
    tg3_parse_options_init(&Options);
    //# Skip image decoding.
    Options.images_as_is = 1;
    //# Parse only what we know.
    Options.skip_extras_values = 1;
    //# Only parse valid trees. Maybe disable in shipping?
    Options.validate_indices = 1;
    //# For external buffers (.gltf files with .bin files next to them).
    Options.fs.read_file = &::ReadExternalFile;
    Options.fs.free_file = &::FreeExternalFile;
    Options.fs.user_data = const_cast<LPath*>(&Directory);

    tg3_error_stack Errors;
    tg3_error_stack_init(&Errors);

    /* External files are resolved by our callback relative to #Directory. Therefore, no base directory here. */
    tg3_error_code const Error{tg3_parse_auto(
          &Out.Native, &Errors
        , reinterpret_cast<uint8_t const*>(Data.data()), Data.size()
        , /* base_dir */ "", 0
        , &Options
        )};
    bool const bFailed{Error != TG3_OK || tg3_errors_has_error(&Errors)};

    ::LogErrors(Path, Errors);
    tg3_error_stack_free(&Errors);

    return bFailed ? LStaticMesh::EResult::LoadingError : LStaticMesh::EResult::Success;
}

Jafg::LStaticMesh::EResult Jafg::Gltf::PullMesh(LModel const& Model, LPath const& Path, u32 MeshIdx, TArray<LStaticMesh::HostMesh>& Out)
{
    STAT_FUNCTION()
    check(Out.empty())

    if (MeshIdx >= Model->meshes_count)
    {
        return LStaticMesh::EResult::NoSuchMesh;
    }

    LStaticMesh::HostMesh Mesh;
    ::PullMesh(*Model, Path, MeshIdx, Mesh);
    if (Mesh.Primitives.empty())
    {
        return LStaticMesh::EResult::Empty;
    }
    Out.push_back(std::move(Mesh));
    return LStaticMesh::EResult::Success;
}

LWorldTrans Jafg::Gltf::GetLocalTransform(tg3_node const& Node, LPath const& Path) noexcept
{
    using LTranslation = decltype(LWorldTrans::t);
    using LRotation = decltype(LWorldTrans::r);
    using LScale = decltype(LWorldTrans::s);

    if (!Node.has_matrix)
    {
        /* glTF stores quaternions as [x, y, z, w]. */
        glm::dquat Rotation{glm::dquat::wxyz(Node.rotation[3], Node.rotation[0], Node.rotation[1], Node.rotation[2])};
        Rotation = glm::length(Rotation) > 0.0 ? glm::normalize(Rotation) : glm::dquat::wxyz(1.0, 0.0, 0.0, 0.0);

        return LWorldTrans{
            .t = LTranslation{
                static_cast<LWorldReal>(Node.translation[0]),
                static_cast<LWorldReal>(Node.translation[1]),
                static_cast<LWorldReal>(Node.translation[2]),
                },
            .r = LRotation::wxyz(
                static_cast<f32>(Rotation.w),
                static_cast<f32>(Rotation.x),
                static_cast<f32>(Rotation.y),
                static_cast<f32>(Rotation.z)
                ),
            .s = LScale{
                static_cast<f32>(Node.scale[0]),
                static_cast<f32>(Node.scale[1]),
                static_cast<f32>(Node.scale[2]),
                },
            };
    }

    /* Column-major, as glm. */
    glm::dmat4 M;
    for (glm::length_t Column{0}; Column < 4; ++Column)
    {
        for (glm::length_t Row{0}; Row < 4; ++Row)
        {
            M[Column][Row] = Node.matrix[Column * 4 + Row];
        }
    }

    glm::dvec3 const Translation{M[3]};
    glm::dvec3 const C0{M[0]};
    glm::dvec3 const C1{M[1]};
    glm::dvec3 const C2{M[2]};
    glm::dvec3 Scale{glm::length(C0), glm::length(C1), glm::length(C2)};
    if (glm::determinant(glm::dmat3{C0, C1, C2}) < 0.0)
    {
        /* Mirrored. Which axis is negative is ambiguous; pick x. */
        Scale.x = -Scale.x;
    }

    glm::dquat Rotation{glm::dquat::wxyz(1.0, 0.0, 0.0, 0.0)};
    if (constexpr double Epsilon{1.0e-12}; glm::abs(Scale.x) > Epsilon && glm::abs(Scale.y) > Epsilon && glm::abs(Scale.z) > Epsilon)
    {
        glm::dmat3 const R{C0 / Scale.x, C1 / Scale.y, C2 / Scale.z};
        if (constexpr double ShearEpsilon{1.0e-4};
               glm::abs(glm::dot(R[0], R[1])) > ShearEpsilon
            || glm::abs(glm::dot(R[0], R[2])) > ShearEpsilon
            || glm::abs(glm::dot(R[1], R[2])) > ShearEpsilon
            )
        {
            LOG_WARNING(LogRhi, "[{}]: Node [{}] has a sheared matrix. The shear is lost."
                , Path, Gltf::ToString(Node.name))
        }
        Rotation = glm::normalize(glm::quat_cast(R));
    }

    return LWorldTrans{
        .t = LTranslation{
            static_cast<LWorldReal>(Translation.x),
            static_cast<LWorldReal>(Translation.y),
            static_cast<LWorldReal>(Translation.z),
            },
        .r = LRotation::wxyz(
            static_cast<f32>(Rotation.w),
            static_cast<f32>(Rotation.x),
            static_cast<f32>(Rotation.y),
            static_cast<f32>(Rotation.z)
            ),
        .s = LScale{
            static_cast<f32>(Scale.x),
            static_cast<f32>(Scale.y),
            static_cast<f32>(Scale.z),
            },
        };
}
