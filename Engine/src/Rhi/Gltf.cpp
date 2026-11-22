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

//#
//# Pulls the geometry of one primitive. Logs and returns nothing if it cannot be drawn.
//# @param MeshName For diagnostics only.
//#
std::optional<Jafg::LStaticMesh::HostPrimitive> PullPrimitive(tg3_model const& Model, LPath const& Path, LStringView MeshName, u32 PrimitiveIdx, tg3_primitive const& SrcPrimitive)
{
    auto PrimitiveMode{SrcPrimitive.mode < 0 ? TG3_MODE_TRIANGLES : SrcPrimitive.mode};
    if (PrimitiveMode != TG3_MODE_TRIANGLES)
    {
        LOG_WARNING(LogRhi, "[{}]: Skipping primitive [{}::{}] with unsupported mode [{}]."
            , Path, MeshName, PrimitiveIdx, PrimitiveMode)
        return {};
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
            , Path, MeshName, PrimitiveIdx)
        return {};
    }

    TArray<f32> Positions;
    TArray<f32> Normals;
    TArray<f32> TexCoords;
    TArray<f32> Tangents;
    if (!::ReadAccessorFloats(Model, PositionAttributeIdx, decltype(Jafg::LStaticMesh::Vertex::Position)::length(), &Positions))
    {
        LOG_WARNING(LogRhi, "[{}]: Skipping primitive [{}::{}] with unreadable POSITION attribute."
            , Path, MeshName, PrimitiveIdx)
        return {};
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
            , Path, MeshName, PrimitiveIdx)
        return {};
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
            , Path, MeshName, PrimitiveIdx)
        return {};
    }

    return Primitive;
}

void PullMesh(tg3_model const& Model, LPath const& Path, u32 MeshIdx, Jafg::LStaticMesh::HostMesh& DstMesh)
{
    check(MeshIdx < Model.meshes_count)
    tg3_mesh const& SrcMesh{Model.meshes[MeshIdx]};

    DstMesh.Name = Jafg::Gltf::ToString(SrcMesh.name);
    DstMesh.Primitives.reserve(SrcMesh.primitives_count);

    for (decltype(SrcMesh.primitives_count) PrimitiveIdx{}; PrimitiveIdx < SrcMesh.primitives_count; ++PrimitiveIdx)
    {
        if (auto Primitive{::PullPrimitive(Model, Path, DstMesh.Name, PrimitiveIdx, SrcMesh.primitives[PrimitiveIdx])})
        {
            DstMesh.Primitives.push_back(std::move(*Primitive));
        }
    }
}

//# The accessor of an attribute of a primitive. Negative if the primitive does not have it.
NODISCARD i32 FindAttribute(tg3_primitive const& Primitive, char const* Name) noexcept
{
    for (decltype(Primitive.attributes_count) Idx{}; Idx < Primitive.attributes_count; ++Idx)
    {
        if (tg3_str_equals_cstr(Primitive.attributes[Idx].key, Name))
        {
            return Primitive.attributes[Idx].value;
        }
    }
    return -1;
}

//# Reads a JOINTS_n accessor (four unsigned bytes or shorts per vertex).
NODISCARD bool ReadJoints(tg3_model const& Model, i32 AccessorIdx, TArray<u16>* Result)
{
    check(Result)

    if (AccessorIdx < 0 || static_cast<decltype(Model.accessors_count)>(AccessorIdx) >= Model.accessors_count)
    {
        return false;
    }
    tg3_accessor const& Accessor{Model.accessors[AccessorIdx]};
    if (Accessor.buffer_view < 0 || tg3_num_components(Accessor.type) != 4)
    {
        return false;
    }
    if (Accessor.component_type != TG3_COMPONENT_TYPE_UNSIGNED_BYTE && Accessor.component_type != TG3_COMPONENT_TYPE_UNSIGNED_SHORT)
    {
        return false;
    }

    tg3_buffer_view const& BufferView{Model.buffer_views[Accessor.buffer_view]};
    tg3_buffer const& Buffer{Model.buffers[BufferView.buffer]};
    auto const Stride{tg3_accessor_byte_stride(&Accessor, &BufferView)};
    if (!Buffer.data.data || Stride <= 0)
    {
        return false;
    }

    uint8_t const* Base{Buffer.data.data + BufferView.byte_offset + Accessor.byte_offset};
    Result->resize(static_cast<std::size_t>(Accessor.count) * 4uz);
    for (decltype(Accessor.count) Idx{}; Idx < Accessor.count; ++Idx)
    {
        uint8_t const* Element{Base + Idx * static_cast<uint64_t>(Stride)};
        for (auto Component{0uz}; Component < 4uz; ++Component)
        {
            u16 Value;
            if (Accessor.component_type == TG3_COMPONENT_TYPE_UNSIGNED_BYTE)
            {
                Value = Element[Component];
            }
            else
            {
                std::memcpy(&Value, Element + Component * sizeof(u16), sizeof(u16));
            }
            (*Result)[static_cast<std::size_t>(Idx) * 4uz + Component] = Value;
        }
    }
    return true;
}

//# The parent of every node. Negative for nodes without one.
NODISCARD TArray<i32> GetNodeParents(tg3_model const& Model)
{
    TArray<i32> Parents(Model.nodes_count, -1);
    for (decltype(Model.nodes_count) Idx{}; Idx < Model.nodes_count; ++Idx)
    {
        tg3_node const& Node{Model.nodes[Idx]};
        for (decltype(Node.children_count) Child{}; Child < Node.children_count; ++Child)
        {
            if (Node.children[Child] >= 0 && static_cast<decltype(Model.nodes_count)>(Node.children[Child]) < Model.nodes_count)
            {
                Parents[static_cast<std::size_t>(Node.children[Child])] = static_cast<i32>(Idx);
            }
        }
    }
    return Parents;
}

NODISCARD LMat4F GetLocalMatrix(tg3_node const& Node, LPath const& Path) noexcept
{
    LWorldTrans const Local{Jafg::Gltf::GetLocalTransform(Node, Path)};
    return glm::translate(maths::identity<LMat4F>, LVec3F{Local.t})
        *  glm::mat4_cast(LQuatF{Local.r})
        *  glm::scale(maths::identity<LMat4F>, LVec3F{Local.s});
}

//# The transform of a node in the space of the file (the product of all its parents and itself).
NODISCARD LMat4F GetGlobalMatrix(tg3_model const& Model, TArray<i32> const& Parents, i32 NodeIdx, LPath const& Path) noexcept
{
    LMat4F Result{maths::identity<LMat4F>};
    for (i32 Idx{NodeIdx}; Idx >= 0; Idx = Parents[static_cast<std::size_t>(Idx)])
    {
        Result = ::GetLocalMatrix(Model.nodes[Idx], Path) * Result;
    }
    return Result;
}

//#
//# Builds the skeleton of a skin. The joints are sorted parents first.
//# @param OutRemap For every joint of the skin (in the order of the file): its index in the skeleton.
//#
NODISCARD bool PullSkeleton(tg3_model const& Model, LPath const& Path, u32 SkinIdx, Jafg::LSkeleton& Out, TArray<u16>& OutRemap)
{
    tg3_skin const& Skin{Model.skins[SkinIdx]};
    if (Skin.joints_count == 0)
    {
        LOG_ERROR(LogRhi, "[{}]: Skin [{}] has no joints.", Path, Jafg::Gltf::ToString(Skin.name))
        return false;
    }
    if (Skin.joints_count > std::numeric_limits<u16>::max())
    {
        LOG_ERROR(LogRhi, "[{}]: Skin [{}] has [{}] joints. At most [{}] are supported."
            , Path, Jafg::Gltf::ToString(Skin.name), Skin.joints_count, std::numeric_limits<u16>::max())
        return false;
    }

    TArray<i32> const Parents{::GetNodeParents(Model)};

    /* Node -> index of the joint in the skin. */
    std::unordered_map<i32, u32> SkinJointOfNode;
    for (decltype(Skin.joints_count) Idx{}; Idx < Skin.joints_count; ++Idx)
    {
        i32 const Node{Skin.joints[Idx]};
        if (Node < 0 || static_cast<decltype(Model.nodes_count)>(Node) >= Model.nodes_count || !SkinJointOfNode.try_emplace(Node, Idx).second)
        {
            LOG_ERROR(LogRhi, "[{}]: Skin [{}] has an invalid or duplicated joint node [{}].", Path, Jafg::Gltf::ToString(Skin.name), Node)
            return false;
        }
    }

    /* The parent joint (as index into the skin) of every joint of the skin. Non-joint nodes in between are skipped. */
    TArray<i32> ParentJoint(Skin.joints_count, -1);
    TArray<i32> RootParentNode(Skin.joints_count, -1);
    u32 SkippedTransforms{};
    for (decltype(Skin.joints_count) Idx{}; Idx < Skin.joints_count; ++Idx)
    {
        i32 Node{Parents[static_cast<std::size_t>(Skin.joints[Idx])]};
        bool bSkipped{};
        while (Node >= 0)
        {
            if (auto const It{SkinJointOfNode.find(Node)}; It != SkinJointOfNode.end())
            {
                ParentJoint[Idx] = static_cast<i32>(It->second);
                break;
            }
            bSkipped = true;
            Node = Parents[static_cast<std::size_t>(Node)];
        }
        if (ParentJoint[Idx] < 0)
        {
            RootParentNode[Idx] = Parents[static_cast<std::size_t>(Skin.joints[Idx])];
        }
        else if (bSkipped)
        {
            ++SkippedTransforms;
        }
    }
    if (SkippedTransforms > 0)
    {
        LOG_WARNING(LogRhi, "[{}]: [{}] joints of skin [{}] hang below nodes that are no joints. Their transforms are ignored."
            , Path, SkippedTransforms, Jafg::Gltf::ToString(Skin.name))
    }

    /* Parents first, depth-first. Siblings keep the order of the file. */
    TArray<TArray<u32>> Children(Skin.joints_count);
    TArray<u32> Roots;
    for (decltype(Skin.joints_count) Idx{}; Idx < Skin.joints_count; ++Idx)
    {
        if (ParentJoint[Idx] >= 0)
        {
            Children[static_cast<std::size_t>(ParentJoint[Idx])].push_back(Idx);
        }
        else
        {
            Roots.push_back(Idx);
        }
    }
    TArray<u32> Order;
    Order.reserve(Skin.joints_count);
    auto const Visit{[&](this auto const& Self, u32 Joint) -> void
    {
        Order.push_back(Joint);
        for (u32 const Child: Children[Joint])
        {
            Self(Child);
        }
    }};
    for (u32 const Root: Roots)
    {
        Visit(Root);
    }
    if (Order.size() != Skin.joints_count)
    {
        LOG_ERROR(LogRhi, "[{}]: The joints of skin [{}] do not form a tree.", Path, Jafg::Gltf::ToString(Skin.name))
        return false;
    }

    OutRemap.assign(Skin.joints_count, 0);
    for (auto Idx{0uz}; Idx < Order.size(); ++Idx)
    {
        OutRemap[Order[Idx]] = static_cast<u16>(Idx);
    }

    TArray<f32> InverseBinds;
    if (Skin.inverse_bind_matrices >= 0)
    {
        if (!::ReadAccessorFloats(Model, Skin.inverse_bind_matrices, 16, &InverseBinds) || InverseBinds.size() < static_cast<std::size_t>(Skin.joints_count) * 16uz)
        {
            LOG_ERROR(LogRhi, "[{}]: Failed to read the inverse bind matrices of skin [{}].", Path, Jafg::Gltf::ToString(Skin.name))
            return false;
        }
    }

    /* Everything above the roots. Should be the same for all roots. */
    if (i32 const Node{RootParentNode[Order.front()]}; Node >= 0)
    {
        Out.RootParent = ::GetGlobalMatrix(Model, Parents, Node, Path);
    }
    for (u32 const Root: Roots)
    {
        if (RootParentNode[Root] != RootParentNode[Order.front()])
        {
            LOG_WARNING(LogRhi, "[{}]: The roots of skin [{}] hang below different nodes. Only the parent of the first root is respected."
                , Path, Jafg::Gltf::ToString(Skin.name))
            break;
        }
    }

    std::unordered_set<LString> Names;
    Out.Joints.resize(Order.size());
    for (auto Idx{0uz}; Idx < Order.size(); ++Idx)
    {
        u32 const SkinJoint{Order[Idx]};
        tg3_node const& Node{Model.nodes[Skin.joints[SkinJoint]]};
        Jafg::LSkeleton::LJoint& Joint{Out.Joints[Idx]};

        Joint.Name = Jafg::Gltf::ToString(Node.name);
        if (Joint.Name.empty() || !Names.insert(Joint.Name).second)
        {
            LString const Unique{algo::sprintf("{}#{}", Joint.Name, Skin.joints[SkinJoint])};
            LOG_WARNING(LogRhi, "[{}]: Joint [{}] of skin [{}] has no unique name. Animations cannot target it. Renamed to [{}]."
                , Path, Joint.Name, Jafg::Gltf::ToString(Skin.name), Unique)
            Joint.Name = Unique;
            Names.insert(Joint.Name);
        }

        Joint.Parent = ParentJoint[SkinJoint] >= 0 ? static_cast<i32>(OutRemap[static_cast<std::size_t>(ParentJoint[SkinJoint])]) : -1;

        LWorldTrans const Local{Jafg::Gltf::GetLocalTransform(Node, Path)};
        Joint.Rest = Jafg::LJointTransform{.T = LVec3F{Local.t}, .R = LQuatF{Local.r}, .S = LVec3F{Local.s}};

        if (!InverseBinds.empty())
        {
            /* Column-major, as glm. */
            for (glm::length_t Column{0}; Column < 4; ++Column)
            {
                for (glm::length_t Row{0}; Row < 4; ++Row)
                {
                    Joint.InverseBind[Column][Row] = InverseBinds[static_cast<std::size_t>(SkinJoint) * 16uz + static_cast<std::size_t>(Column * 4 + Row)];
                }
            }
        }
    }

    Out.RebuildLookup();
    check(Out.IsValid())
    return true;
}

//#
//# Pulls the skinning data of a primitive that was pulled with #PullPrimitive. At most four influences per vertex
//# are kept (the strongest); the weights are normalized.
//# @param Remap From the joints of the skin to the joints of the skeleton.
//#
NODISCARD bool PullInfluences(
      tg3_model const& Model
    , LPath const& Path
    , tg3_primitive const& SrcPrimitive
    , TArray<u16> const& Remap
    , std::size_t VertexCount
    , TArray<Jafg::LSkinInfluence>& Out
    , u32& OutUnweighted
    )
{
    struct LSet final
    {
        TArray<u16> Joints;
        TArray<f32> Weights;
    };
    TArray<LSet> Sets;
    for (u32 SetIdx{0};; ++SetIdx)
    {
        LString const JointsName{algo::sprintf("JOINTS_{}", SetIdx)};
        LString const WeightsName{algo::sprintf("WEIGHTS_{}", SetIdx)};
        i32 const JointsIdx{::FindAttribute(SrcPrimitive, JointsName.c_str())};
        i32 const WeightsIdx{::FindAttribute(SrcPrimitive, WeightsName.c_str())};
        if (JointsIdx < 0 || WeightsIdx < 0)
        {
            break;
        }

        LSet Set;
        if (!::ReadJoints(Model, JointsIdx, &Set.Joints) || !::ReadAccessorFloats(Model, WeightsIdx, 4, &Set.Weights)
            || Set.Joints.size() != VertexCount * 4uz || Set.Weights.size() != VertexCount * 4uz)
        {
            LOG_ERROR(LogRhi, "[{}]: Failed to read [{}] and [{}].", Path, JointsName, WeightsName)
            return false;
        }
        Sets.push_back(std::move(Set));
    }
    if (Sets.empty())
    {
        LOG_ERROR(LogRhi, "[{}]: A skinned primitive has no JOINTS_0 and WEIGHTS_0.", Path)
        return false;
    }

    Out.resize(VertexCount);
    std::vector<std::pair<f32, u16>> Candidates;
    for (auto Vertex{0uz}; Vertex < VertexCount; ++Vertex)
    {
        Candidates.clear();
        for (LSet const& Set: Sets)
        {
            for (auto Slot{0uz}; Slot < 4uz; ++Slot)
            {
                f32 const Weight{Set.Weights[Vertex * 4uz + Slot]};
                u16 const Joint{Set.Joints[Vertex * 4uz + Slot]};
                if (Weight > 0.0f && Joint < Remap.size())
                {
                    Candidates.emplace_back(Weight, Remap[Joint]);
                }
            }
        }
        std::ranges::sort(Candidates, std::greater{});

        Jafg::LSkinInfluence& Influence{Out[Vertex]};
        f32 Sum{};
        for (auto Slot{0uz}; Slot < 4uz && Slot < Candidates.size(); ++Slot)
        {
            Influence.Weights[Slot] = Candidates[Slot].first;
            Influence.Joints[Slot] = Candidates[Slot].second;
            Sum += Candidates[Slot].first;
        }
        if (Sum > 0.0f)
        {
            for (f32& Weight: Influence.Weights)
            {
                Weight /= Sum;
            }
        }
        else
        {
            /* Undefined by glTF. Let the vertex follow the first root. */
            Influence.Joints[0] = 0;
            Influence.Weights[0] = 1.0f;
            ++OutUnweighted;
        }
    }
    return true;
}

NODISCARD std::optional<Jafg::EAnimationPath> ToAnimationPath(tg3_str const& Path) noexcept
{
    if (tg3_str_equals_cstr(Path, "translation"))
    {
        return Jafg::EAnimationPath::Translation;
    }
    if (tg3_str_equals_cstr(Path, "rotation"))
    {
        return Jafg::EAnimationPath::Rotation;
    }
    if (tg3_str_equals_cstr(Path, "scale"))
    {
        return Jafg::EAnimationPath::Scale;
    }
    return {};
}

NODISCARD Jafg::EAnimationInterpolation ToAnimationInterpolation(tg3_str const& Interpolation) noexcept
{
    if (tg3_str_equals_cstr(Interpolation, "STEP"))
    {
        return Jafg::EAnimationInterpolation::Step;
    }
    if (tg3_str_equals_cstr(Interpolation, "CUBICSPLINE"))
    {
        return Jafg::EAnimationInterpolation::CubicSpline;
    }
    /* The default. */
    return Jafg::EAnimationInterpolation::Linear;
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

Jafg::LStaticMesh::EResult Jafg::Gltf::PullSkeletalMesh(LModel const& Model, LPath const& Path, LSkeletalMesh& Out)
{
    STAT_FUNCTION()
    check(Out.Primitives.empty())

    TArray<i32> const Parents{::GetNodeParents(*Model)};

    /* The roots of the default scene (or the first one). All parentless nodes if the file has no scenes. */
    TArray<i32> Roots;
    if (Model->scenes_count > 0)
    {
        u32 const SceneIdx{Model->default_scene >= 0 && static_cast<u32>(Model->default_scene) < Model->scenes_count
            ? static_cast<u32>(Model->default_scene) : 0u};
        tg3_scene const& Scene{Model->scenes[SceneIdx]};
        Roots.assign(Scene.nodes, Scene.nodes + Scene.nodes_count);
    }
    else
    {
        for (auto Idx{0uz}; Idx < Parents.size(); ++Idx)
        {
            if (Parents[Idx] < 0)
            {
                Roots.push_back(static_cast<i32>(Idx));
            }
        }
    }

    /* Every node with a skinned mesh, depth-first. */
    TArray<i32> SkinnedNodes;
    auto const Walk{[&](this auto const& Self, i32 NodeIdx) -> void
    {
        if (NodeIdx < 0 || static_cast<u32>(NodeIdx) >= Model->nodes_count)
        {
            return;
        }
        tg3_node const& Node{Model->nodes[NodeIdx]};
        if (Node.mesh >= 0 && Node.skin >= 0
            && static_cast<u32>(Node.mesh) < Model->meshes_count && static_cast<u32>(Node.skin) < Model->skins_count)
        {
            SkinnedNodes.push_back(NodeIdx);
        }
        for (decltype(Node.children_count) Child{}; Child < Node.children_count; ++Child)
        {
            Self(Node.children[Child]);
        }
    }};
    for (i32 const Root: Roots)
    {
        Walk(Root);
    }
    if (SkinnedNodes.empty())
    {
        LOG_ERROR(LogRhi, "[{}]: The scene has no skinned mesh (a node with a mesh and a skin).", Path)
        return LStaticMesh::EResult::NoSuchMesh;
    }

    /* All meshes that share the skin of the first one, e.g., a body and its clothes. */
    u32 const SkinIdx{static_cast<u32>(Model->nodes[SkinnedNodes.front()].skin)};
    auto Skeleton{std::make_shared<LSkeleton>()};
    TArray<u16> Remap;
    if (!::PullSkeleton(*Model, Path, SkinIdx, *Skeleton, Remap))
    {
        return LStaticMesh::EResult::LoadingError;
    }

    std::unordered_set<i32> PulledMeshes;
    u32 OtherSkins{};
    u32 Unweighted{};
    for (i32 const NodeIdx: SkinnedNodes)
    {
        tg3_node const& Node{Model->nodes[NodeIdx]};
        if (static_cast<u32>(Node.skin) != SkinIdx)
        {
            ++OtherSkins;
            continue;
        }
        if (!PulledMeshes.insert(Node.mesh).second)
        {
            /* The same mesh twice is the same geometry twice. */
            continue;
        }

        tg3_mesh const& SrcMesh{Model->meshes[Node.mesh]};
        LString const MeshName{Gltf::ToString(SrcMesh.name)};
        if (Out.Name.empty())
        {
            Out.Name = MeshName;
        }

        for (decltype(SrcMesh.primitives_count) PrimitiveIdx{}; PrimitiveIdx < SrcMesh.primitives_count; ++PrimitiveIdx)
        {
            tg3_primitive const& SrcPrimitive{SrcMesh.primitives[PrimitiveIdx]};
            auto Primitive{::PullPrimitive(*Model, Path, MeshName, PrimitiveIdx, SrcPrimitive)};
            if (!Primitive)
            {
                continue;
            }

            TArray<LSkinInfluence> Influences;
            if (!::PullInfluences(*Model, Path, SrcPrimitive, Remap, Primitive->Vertices.size(), Influences, Unweighted))
            {
                LOG_WARNING(LogRhi, "[{}]: Skipping primitive [{}::{}] without readable skinning data.", Path, MeshName, PrimitiveIdx)
                continue;
            }

            auto& Dst{Out.Primitives.emplace_back()};
            Dst.Vertices = std::move(Primitive->Vertices);
            Dst.Influences = std::move(Influences);
            Dst.Indices = std::move(Primitive->Indices);
        }
    }

    if (OtherSkins > 0)
    {
        LOG_WARNING(LogRhi, "[{}]: [{}] skinned nodes use another skin than the first one. They are ignored.", Path, OtherSkins)
    }
    if (Unweighted > 0)
    {
        LOG_WARNING(LogRhi, "[{}]: [{}] vertices have no joint weights. They follow the root.", Path, Unweighted)
    }
    if (Out.Primitives.empty())
    {
        return LStaticMesh::EResult::Empty;
    }

    Out.Skeleton = std::move(Skeleton);
    return LStaticMesh::EResult::Success;
}

Jafg::LStaticMesh::EResult Jafg::Gltf::PullAnimations(LModel const& Model, LPath const& Path, TArray<LAnimationClipRef>& Out)
{
    STAT_FUNCTION()

    std::unordered_set<LString> Names;
    for (decltype(Model->animations_count) AnimationIdx{}; AnimationIdx < Model->animations_count; ++AnimationIdx)
    {
        tg3_animation const& Animation{Model->animations[AnimationIdx]};

        auto Clip{std::make_shared<LAnimationClip>()};
        Clip->Name = Gltf::ToString(Animation.name);
        if (Clip->Name.empty())
        {
            Clip->Name = algo::sprintf("Animation{}", AnimationIdx);
        }

        f32 Start{std::numeric_limits<f32>::max()};
        f32 End{std::numeric_limits<f32>::lowest()};
        u32 Unsupported{};
        u32 Broken{};
        for (decltype(Animation.channels_count) ChannelIdx{}; ChannelIdx < Animation.channels_count; ++ChannelIdx)
        {
            tg3_animation_channel const& SrcChannel{Animation.channels[ChannelIdx]};
            i32 const NodeIdx{SrcChannel.target.node};
            std::optional<EAnimationPath> const AnimationPath{::ToAnimationPath(SrcChannel.target.path)};
            if (NodeIdx < 0 || static_cast<u32>(NodeIdx) >= Model->nodes_count || !AnimationPath)
            {
                /* E.g., morph target weights. */
                ++Unsupported;
                continue;
            }
            if (SrcChannel.sampler < 0 || static_cast<u32>(SrcChannel.sampler) >= Animation.samplers_count)
            {
                ++Broken;
                continue;
            }
            tg3_animation_sampler const& Sampler{Animation.samplers[SrcChannel.sampler]};

            LAnimationChannel Channel{
                .Target = Gltf::ToString(Model->nodes[NodeIdx].name),
                .Path = *AnimationPath,
                .Interpolation = ::ToAnimationInterpolation(Sampler.interpolation),
                };
            if (Channel.Target.empty())
            {
                /* Clips bind by name. */
                ++Unsupported;
                continue;
            }

            TArray<f32> Values;
            if (!::ReadAccessorFloats(*Model, Sampler.input, 1, &Channel.Times) || !::ReadAccessorFloats(*Model, Sampler.output, 4, &Values))
            {
                ++Broken;
                continue;
            }
            std::size_t const Stride{Channel.Interpolation == EAnimationInterpolation::CubicSpline ? 3uz : 1uz};
            if (Channel.Times.empty() || Values.size() != Channel.Times.size() * Stride * 4uz || !std::ranges::is_sorted(Channel.Times))
            {
                ++Broken;
                continue;
            }

            Channel.Values.resize(Channel.Times.size() * Stride);
            for (auto Idx{0uz}; Idx < Channel.Values.size(); ++Idx)
            {
                Channel.Values[Idx] = LVec4F{Values[Idx * 4 + 0], Values[Idx * 4 + 1], Values[Idx * 4 + 2], Values[Idx * 4 + 3]};
            }

            Start = maths::min(Start, Channel.Times.front());
            End = maths::max(End, Channel.Times.back());
            Clip->Channels.push_back(std::move(Channel));
        }

        if (Broken > 0)
        {
            LOG_WARNING(LogRhi, "[{}]: [{}] channels of animation [{}] are broken and dropped.", Path, Broken, Clip->Name)
        }
        if (Unsupported > 0)
        {
            LOG_VERBOSE(LogRhi, "[{}]: [{}] channels of animation [{}] are not supported (morph weights or unnamed nodes) and dropped."
                , Path, Unsupported, Clip->Name)
        }
        if (Clip->Channels.empty())
        {
            LOG_WARNING(LogRhi, "[{}]: Animation [{}] has no usable channel. Skipping it.", Path, Clip->Name)
            continue;
        }
        if (!Names.insert(Clip->Name).second)
        {
            LOG_WARNING(LogRhi, "[{}]: Animation [{}] exists more than once. Only the first one can be found by its name.", Path, Clip->Name)
        }

        /* Every clip starts at zero. */
        for (auto& Channel: Clip->Channels)
        {
            for (f32& Time: Channel.Times)
            {
                Time -= Start;
            }
        }
        Clip->Duration = End - Start;
        Out.push_back(std::move(Clip));
    }

    return Out.empty() ? LStaticMesh::EResult::Empty : LStaticMesh::EResult::Success;
}
