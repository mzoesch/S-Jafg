// Copyright mzoesch. All rights reserved.

#include "Framework/MeshSubsystem.h"
#include "../Rhi/Gltf.h"

void Jafg::JMeshSubsystem::PurgeUnused() noexcept
{
    std::erase_if(this->Meshes, [](auto& E) -> bool
    {
        if (E.second.use_count() == 1)
        {
            LOG_VERBOSE(LogRhi, "Purging unused mesh [{}].", E.first)
            return true;
        }

        return false;
    });

    // TODO: Do we want to rehash??
}

namespace
{

NODISCARD bool IsNearlyIdentity(LWorldTrans const& Transform) noexcept
{
    constexpr f32 Epsilon{1.0e-5f};
    return maths::magnitude(LVec3F{Transform.t}) < Epsilon
        && maths::abs(maths::abs(Transform.r.w) - 1.0f) < Epsilon
        && maths::magnitude(LVec3F{Transform.s} - LVec3F{1.0f}) < Epsilon;
}

} /* ~Namespace <Anonymous> */

LPath Jafg::JMeshSubsystem::NormalizePath(LPath const& Path) noexcept
{
    std::error_code Error;
    LPath Absolute{std::filesystem::absolute(Path, Error)};
    if (Error)
    {
        LOG_WARNING(LogRhi, "[{}]: Failed to make path absolute: {}.", Path, Error.message())
        return Path.lexically_normal();
    }
    return Absolute.lexically_normal();
}

Jafg::LStaticMeshRef Jafg::JMeshSubsystem::FromSource(LStaticMeshSource const& Source, EStaticMeshState State, std::optional<LStaticMeshCreateInfo> const& Info) const
{
    auto& Mesh{this->FindOrAllocate(Source, Info)};
    if (auto Result{Mesh->BringToState(State)}; Result != LStaticMesh::EResult::Success)
    {
        LOG_FATAL(LogRhi, "[{}]: Failed to bring mesh to the requested state: [{}]."
            , Source, std::to_underlying(Result))
    }
    return Mesh;
}

std::shared_ptr<Jafg::LStaticMesh>& Jafg::JMeshSubsystem::FindOrAllocate(LStaticMeshSource const& Source, std::optional<LStaticMeshCreateInfo> const& Info) const
{
    auto& Mesh{this->Meshes[Source]};
    if (!Mesh)
    {
        Mesh = std::make_shared<LStaticMesh>(Source);
    }
    if (Info)
    {
        Mesh->MaxConvexVertices = Info->MaxConvexVertices;
        Mesh->ConvexTolerance = Info->ConvexTolerance;
    }
    return Mesh;
}

Jafg::LStaticMeshRef Jafg::JMeshSubsystem::LoadMesh(LPath const& Path, EStaticMeshState State /* = EStaticMeshStateBits::Device */, std::optional<LStaticMeshCreateInfo> Info /* = {} */) const
{
    STAT_FUNCTION()

    LPath const Normalized{NormalizePath(Path)};

    /* Fast path. Known file; the file is only read if the mesh has to be brought to a new state. */
    if (auto It{this->SingleMeshIndices.find(Normalized)}; It != this->SingleMeshIndices.end())
    {
        return this->FromSource(LStaticMeshSource{.Path = Normalized, .MeshIdx = It->second}, State, Info);
    }

    auto Scene{this->LoadScene(Normalized, {}, State, std::move(Info))};
    if (!Scene)
    {
        LOG_FATAL(LogRhi, "[{}]: Failed to load mesh: [{}].", Normalized, std::to_underlying(Scene.error()))
    }

    /* Exactly one mesh. Remember the first node that uses it and whether any transform is on the way to it. */
    LMeshSceneNode const* MeshNode{};
    bool bTransformed{};
    std::size_t MeshNodeCount{};
    std::unordered_set<LStaticMesh const*> Meshes;
    auto const Walk{[&](this auto const& Self, TArray<LMeshSceneNode> const& Nodes, bool bParentTransformed) -> void
    {
        for (auto const& Node: Nodes)
        {
            bool const bNodeTransformed{bParentTransformed || !::IsNearlyIdentity(Node.Transform)};
            if (Node.Mesh)
            {
                Meshes.insert(Node.Mesh.get());
                ++MeshNodeCount;
                if (!MeshNode)
                {
                    MeshNode = &Node;
                    bTransformed = bNodeTransformed;
                }
            }
            Self(Node.Children, bNodeTransformed);
        }
    }};
    Walk(Scene->Nodes, false);

    if (Meshes.size() != 1uz)
    {
        LOG_FATAL(LogRhi, "[{}]: Expected exactly one mesh, but the scene has [{}]. Use LoadScene (or LoadAndProjectScene) for this file."
            , Normalized, Meshes.size())
    }
    check(MeshNode && MeshNode->Mesh)

    if (bTransformed)
    {
        LOG_WARNING(LogRhi, "[{}]: The node [{}] of the mesh (or one of its parents) has a transform. It is ignored. Use LoadScene to respect it."
            , Normalized, MeshNode->Name)
    }
    if (MeshNodeCount > 1uz)
    {
        LOG_WARNING(LogRhi, "[{}]: The mesh is used by [{}] nodes. Only one instance of it is loaded. Use LoadScene to get all of them."
            , Normalized, MeshNodeCount)
    }

    this->SingleMeshIndices.emplace(Normalized, MeshNode->Mesh->GetSource().MeshIdx);
    return MeshNode->Mesh;
}

Jafg::LMeshSceneProjection Jafg::JMeshSubsystem::LoadAndProjectScene(
      LPath const& Path
    , ASceneComponent& Parent
    , LMeshSceneProjectionInfo const& Projection /* = {} */
    , EStaticMeshState State /* = EStaticMeshStateBits::Device */
    , std::optional<u32> SceneIdx /* = {} */
    , std::optional<LStaticMeshCreateInfo> Info /* = {} */
    ) const
{
    auto Scene{this->LoadScene(Path, SceneIdx, State, std::move(Info))};
    if (!Scene)
    {
        LOG_FATAL(LogRhi, "[{}]: Failed to load scene: [{}].", Path, std::to_underlying(Scene.error()))
    }
    return ProjectMeshScene(*Scene, Parent, Projection);
}

std::expected<Jafg::LMeshScene, Jafg::LStaticMesh::EResult> Jafg::JMeshSubsystem::LoadScene(
      LPath const& InPath
    , std::optional<u32> SceneIdx /* = {} */
    , EStaticMeshState State /* = EStaticMeshStateBits::Device */
    , std::optional<LStaticMeshCreateInfo> Info /* = {} */
    ) const
{
    STAT_FUNCTION()

    /* Everything below (and the cache keys) use the normalized path. */
    LPath const Path{NormalizePath(InPath)};
    LOG_VERBOSE(LogRhi, "[{}]: Loading scene [{}].", Path, SceneIdx ? algo::sprintf("{}", *SceneIdx) : "<default>")

    if (!is_regular_file(Path))
    {
        return std::unexpected{LStaticMesh::EResult::FileNotFound};
    }

    Gltf::LModel Model;
    if (auto Result{Gltf::Parse(Path, Model)}; Result != LStaticMesh::EResult::Success)
    {
        return std::unexpected{Result};
    }

    LMeshScene Scene;
    Scene.Path = Path;

    /* The root nodes of the scene. */
    TArray<u32> Roots;
    if (SceneIdx)
    {
        if (*SceneIdx >= Model->scenes_count)
        {
            LOG_ERROR(LogRhi, "[{}]: No scene [{}]. The file has [{}] scenes.", Path, *SceneIdx, Model->scenes_count)
            return std::unexpected{LStaticMesh::EResult::NoSuchScene};
        }
        Scene.SceneIdx = *SceneIdx;
    }
    else if (Model->default_scene >= 0)
    {
        Scene.SceneIdx = static_cast<u32>(Model->default_scene);
    }
    else if (Model->scenes_count > 0)
    {
        Scene.SceneIdx = 0;
    }

    if (Scene.SceneIdx)
    {
        tg3_scene const& Src{Model->scenes[*Scene.SceneIdx]};
        Scene.Name = Gltf::ToString(Src.name);
        Roots.assign(Src.nodes, Src.nodes + Src.nodes_count);
    }
    else
    {
        LOG_VERBOSE(LogRhi, "[{}]: The file has no scenes. Using all nodes without a parent as roots.", Path)
        TArray<u8> bHasParent(Model->nodes_count, 0);
        for (decltype(Model->nodes_count) Idx{}; Idx < Model->nodes_count; ++Idx)
        {
            tg3_node const& Node{Model->nodes[Idx]};
            for (decltype(Node.children_count) ChildIdx{}; ChildIdx < Node.children_count; ++ChildIdx)
            {
                bHasParent[static_cast<size_t>(Node.children[ChildIdx])] = 1;
            }
        }
        for (decltype(Model->nodes_count) Idx{}; Idx < Model->nodes_count; ++Idx)
        {
            if (!bHasParent[Idx])
            {
                Roots.push_back(Idx);
            }
        }
    }

    /* Meshes referenced by many nodes are brought to the state only once. */
    std::unordered_map<u32, LStaticMeshRef> MeshesOfScene;
    auto GetMesh{[&](u32 MeshIdx) -> LStaticMeshRef
    {
        if (auto It{MeshesOfScene.find(MeshIdx)}; It != MeshesOfScene.end())
        {
            return It->second;
        }

        LStaticMeshSource const Source{.Path = Path, .MeshIdx = MeshIdx};
        auto& Mesh{this->FindOrAllocate(Source, Info)};
        if (auto Result{Mesh->BringToState(State, [&](TArray<LStaticMesh::HostMesh>& Out) -> LStaticMesh::EResult
            {
                return Gltf::PullMesh(Model, Path, MeshIdx, Out);
            })}; Result != LStaticMesh::EResult::Success)
        {
            LOG_WARNING(LogRhi, "[{}]: Failed to load mesh. Its nodes are kept without a mesh: [{}]."
                , Source, std::to_underlying(Result))
            return MeshesOfScene[MeshIdx] = nullptr;
        }
        return MeshesOfScene[MeshIdx] = Mesh;
    }};

    /* glTF requires nodes to form disjoint trees. Guard against malformed files anyway, a cycle would never end. */
    TArray<u8> bVisited(Model->nodes_count, 0);
    auto BuildNode{[&](this auto const& Self, u32 NodeIdx, TArray<LMeshSceneNode>& Siblings) -> void
    {
        check(NodeIdx < Model->nodes_count)
        if (bVisited[NodeIdx])
        {
            LOG_WARNING(LogRhi, "[{}]: Node [{}] is referenced more than once in the scene. Skipping it.", Path, NodeIdx)
            return;
        }
        bVisited[NodeIdx] = 1;

        tg3_node const& Src{Model->nodes[NodeIdx]};
        LMeshSceneNode& Dst{Siblings.emplace_back()};
        Dst.NodeIdx = NodeIdx;
        Dst.Name = Gltf::ToString(Src.name);
        Dst.Transform = Gltf::GetLocalTransform(Src, Path);
        if (Src.mesh >= 0)
        {
            Dst.Mesh = GetMesh(static_cast<u32>(Src.mesh));
        }

        /* Only #Dst.Children grows below. #Dst itself stays where it is. */
        Dst.Children.reserve(Src.children_count);
        for (decltype(Src.children_count) ChildIdx{}; ChildIdx < Src.children_count; ++ChildIdx)
        {
            Self(static_cast<u32>(Src.children[ChildIdx]), Dst.Children);
        }
    }};

    Scene.Nodes.reserve(Roots.size());
    for (u32 const Root: Roots)
    {
        BuildNode(Root, Scene.Nodes);
    }

    LOG_VERBOSE(LogRhi, "[{}]: Loaded scene [{}] with [{}] root nodes and [{}] meshes."
        , Path, Scene.Name, Scene.Nodes.size(), MeshesOfScene.size())

    return Scene;
}
