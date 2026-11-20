// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/StaticMesh.h"

namespace Jafg
{

//# A node of a #LMeshScene. Mirrors a glTF node.
struct LMeshSceneNode final
{
    //#
    //# The index of the node in the source file. Animations target nodes by this index.
    //# Only stable as long as the file is not re-exported.
    //#
    u32 NodeIdx{};
    LString Name;
    //# Relative to the parent node; or to the scene if this is a root node.
    LWorldTrans Transform{ maths::identity<LWorldTrans> };
    //# Null if the node has no mesh (e.g., an empty that only carries a transform for its children or as a socket).
    LStaticMeshRef Mesh;
    TArray<LMeshSceneNode> Children;
};

//#
//# A scene of a glTF file: A hierarchy of nodes with meshes. Every glTF mesh is its own #LStaticMesh. A mesh
//# that is referenced by many nodes is shared.
//#
//# Only the geometry and the node hierarchy are read. Materials, textures, cameras, lights, skins and
//# animations are ignored.
//#
//# @see #JMeshSubsystem::LoadScene to load one.
//# @see #ProjectMeshScene to create components from one.
//#
struct LMeshScene final
{
    LPath Path;
    //# The index of the scene in the source file. Empty if the file has no scenes (all parentless nodes are the roots).
    std::optional<u32> SceneIdx;
    LString Name;
    //# The root nodes.
    TArray<LMeshSceneNode> Nodes;

    //# Depth-first. Null if there is no node with this name. Names are not required to be unique by glTF.
    NODISCARD LMeshSceneNode const* FindNode(LStringView NodeName) const noexcept
    {
        auto const Find{[&](this auto const& Self, TArray<LMeshSceneNode> const& Siblings) -> LMeshSceneNode const*
        {
            for (auto const& Node: Siblings)
            {
                if (Node.Name == NodeName)
                {
                    return &Node;
                }
                if (auto const* Result{Self(Node.Children)})
                {
                    return Result;
                }
            }
            return nullptr;
        }};
        return Find(this->Nodes);
    }
};

} /* ~Namespace Jafg */
