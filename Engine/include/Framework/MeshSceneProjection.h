// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/MeshScene.h"
#include "Rhi/Material.h"
#include "Rhi/PhysicalRendering.h"
#include "Framework/PhysicsCore.h"
#include "Framework/PhysicsBody.h"

namespace Jafg
{

class LWorld;
class AActor;
class ASceneComponent;
class ARigidComponent;
class AStaticMeshComponent;

struct LMeshSceneProjectionInfo final
{
    //# The material of every static mesh component. glTF materials are not read (yet).
    LPbrMaterialInstanceRef PbrMaterialInstance;
    //# Used if #PbrMaterialInstance is not set.
    LMaterialInstanceRef MaterialInstance;
    //#
    //# Called for every component that is created for a node. Right after its name, transform, mesh and
    //# material were set; before its children are projected. Use it to override the material, etc.
    //# Static mesh components for nodes with a mesh, plain scene components for nodes without one.
    //#
    TFunction<void(ASceneComponent& Comp, LMeshSceneNode const& Node)> OnNodeProjected;
};

//# The components that a projection created.
struct LMeshSceneProjection final
{
    struct LNode final
    {
        u32 NodeIdx;
        LString Name;
        ASceneComponent* Component;
    };

    //# One per root node of the scene. In order.
    TArray<ASceneComponent*> Roots;
    //# One per projected node, depth-first in scene order.
    TArray<LNode> Nodes;

    //# Null if no node with this name was projected.
    NODISCARD ASceneComponent* FindByName(LStringView Name) const noexcept
    {
        auto const It{algo::find(this->Nodes, Name, &LNode::Name)};
        return It == this->Nodes.end() ? nullptr : It->Component;
    }
    //# Null if the node was not projected.
    NODISCARD ASceneComponent* FindByNodeIdx(u32 NodeIdx) const noexcept
    {
        auto const It{algo::find(this->Nodes, NodeIdx, &LNode::NodeIdx)};
        return It == this->Nodes.end() ? nullptr : It->Component;
    }
    //# Every static mesh component that was created.
    NODISCARD ENGINE_API TArray<AStaticMeshComponent*> GetStaticMeshComponents() const noexcept;
};

//#
//# Projects a scene into an existing actor below the given component. Every node becomes a component with the
//# local transform of the node: Nodes with a mesh become static mesh components, nodes without one plain scene
//# components (to keep the transforms of their children). The hierarchy of the scene is kept.
//#
//# Works for living actors as well as inside the callback of a deferred spawn.
//#
//# @param Parent Must be attached to the actor. E.g., its root.
//#
ENGINE_API LMeshSceneProjection ProjectMeshScene(LMeshScene const& Scene, ASceneComponent& Parent, LMeshSceneProjectionInfo const& Info = {});

struct LMeshSceneRigidInfo final
{
    EPhysicsMotion Motion{ EPhysicsMotion::Static };
    EPhysicsLayer Layer{ EPhysicsLayer::Static };
    //# Whether to make one convex hull around all projected meshes. The meshes should be loaded with #EStaticMeshStateBits::Convex.
    bool bMakeConvex{ true };
    f32 ConvexRadius{ Physx::DefaultConvexRadius };
    //# Empty: The body is not added to the simulation. Requires #bMakeConvex.
    std::optional<ESimulationAddingBehavior> AddToSimulation{ ESimulationAddingBehavior::Preserve };
};

//#
//# Spawns a new actor into the world and projects the scene below its root. @see #ProjectMeshScene
//#
//# @param Rigid Set: The root is a rigid component (rigid components can only be roots). Empty: A plain scene
//#              component.
//# @param OutProjection Optional. The created components.
//#
ENGINE_API AActor& SpawnMeshScene(
      LMeshScene const& Scene
    , LWorld& World
    , LWorldTrans const& Transform
    , std::optional<LMeshSceneRigidInfo> const& Rigid = LMeshSceneRigidInfo{}
    , LMeshSceneProjectionInfo const& Info = {}
    , LMeshSceneProjection* OutProjection = nullptr
    );

} /* ~Namespace Jafg */
