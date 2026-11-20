// Copyright mzoesch. All rights reserved.

#include "Framework/MeshSceneProjection.h"
#include "Engine/World.h"
#include "Framework/Actor.h"
#include "Framework/SceneComponent.h"
#include "Framework/StaticMeshComponent.h"
#include "Framework/RigidComponent.h"

TArray<Jafg::AStaticMeshComponent*> Jafg::LMeshSceneProjection::GetStaticMeshComponents() const noexcept
{
    TArray<AStaticMeshComponent*> Result;
    for (auto const& Node: this->Nodes)
    {
        if (auto* Comp{DynamicCast<AStaticMeshComponent>(Node.Component)})
        {
            Result.push_back(Comp);
        }
    }
    return Result;
}

Jafg::LMeshSceneProjection Jafg::ProjectMeshScene(LMeshScene const& Scene, ASceneComponent& Parent, LMeshSceneProjectionInfo const& Info /* = {} */)
{
    STAT_FUNCTION()

    LMeshSceneProjection Result;
    bool bWarnedAboutMaterial{false};

    auto const Setup{[&](ASceneComponent& Comp, LMeshSceneNode const& Node)
    {
#if JAFG_WITH_EDITOR
        Comp.EditorName = Node.Name;
#endif /* JAFG_WITH_EDITOR */
        Comp.SetLocalTransformByTeleport(Node.Transform);
    }};

    auto const Project{[&](this auto const& Self, LMeshSceneNode const& Node, ASceneComponent& Into) -> ASceneComponent&
    {
        ASceneComponent* Comp;
        if (Node.Mesh)
        {
            Comp = &Into.EmplaceSceneComponent<AStaticMeshComponent>([&](AStaticMeshComponent& Mesh)
            {
                Setup(Mesh, Node);
                Mesh.SetMesh(Node.Mesh);
                if (Info.PbrMaterialInstance)
                {
                    Mesh.SetPbrMaterialInstance(Info.PbrMaterialInstance);
                }
                else if (Info.MaterialInstance)
                {
                    Mesh.SetMaterialInstance(Info.MaterialInstance);
                }
                else if (!Info.OnNodeProjected.IsValid() && !bWarnedAboutMaterial)
                {
                    bWarnedAboutMaterial = true;
                    LOG_WARNING(LogRhi, "[{}]: No material given for the static meshes of the scene.", Scene.Path)
                }
                Info.OnNodeProjected.InvokeIfBound(Mesh, Node);
            });
        }
        else
        {
            Comp = &Into.EmplaceSceneComponent<ASceneComponent>([&](ASceneComponent& Empty)
            {
                Setup(Empty, Node);
                /* Plain scene components render nothing themselves, but their children. */
                Empty.SetShouldRender(true);
                Info.OnNodeProjected.InvokeIfBound(Empty, Node);
            });
        }
        check(Comp)

        Result.Nodes.push_back(LMeshSceneProjection::LNode{.NodeIdx = Node.NodeIdx, .Name = Node.Name, .Component = Comp});
        for (auto const& Child: Node.Children)
        {
            Self(Child, *Comp);
        }
        return *Comp;
    }};

    Result.Roots.reserve(Scene.Nodes.size());
    for (auto const& Root: Scene.Nodes)
    {
        Result.Roots.push_back(&Project(Root, Parent));
    }

    return Result;
}

Jafg::AActor& Jafg::SpawnMeshScene(
      LMeshScene const& Scene
    , LWorld& World
    , LWorldTrans const& Transform
    , std::optional<LMeshSceneRigidInfo> const& Rigid /* = LMeshSceneRigidInfo{} */
    , LMeshSceneProjectionInfo const& Info /* = {} */
    , LMeshSceneProjection* OutProjection /* = nullptr */
    )
{
    STAT_FUNCTION()
    check(!Rigid || Rigid->bMakeConvex || !Rigid->AddToSimulation)

    LMeshSceneProjection Projection;
    ARigidComponent* RigidRoot{};

    AActor& Actor{*SpawnDeferredObject(TWorldStaticInit<AActor>{World})([&](AActor& Spawned)
    {
#if JAFG_WITH_EDITOR
        Spawned.EditorName = Scene.Path.stem().string();
#endif /* JAFG_WITH_EDITOR */

        ASceneComponent* Root;
        if (Rigid)
        {
            RigidRoot = &Spawned.EmplaceRootComponent<ARigidComponent>([&](ARigidComponent& Comp)
            {
                Comp.SetPhysicsMotion(Rigid->Motion);
                Comp.SetPhysicsLayer(Rigid->Layer);
            });
            Root = RigidRoot;
        }
        else
        {
            Root = &Spawned.EmplaceRootComponent<ASceneComponent>([](ASceneComponent& Comp)
            {
                Comp.SetShouldRender(true);
            });
        }
        Root->SetLocalTransformByTeleport(Transform);

        Projection = ProjectMeshScene(Scene, *Root, Info);
    })};

    if (RigidRoot)
    {
        if (Rigid->bMakeConvex)
        {
            RigidRoot->MakeConvex(Projection.GetStaticMeshComponents(), Rigid->ConvexRadius);
        }
        if (Rigid->AddToSimulation && RigidRoot->GetRigidObject().IsValid())
        {
            RigidRoot->AddToSimulation(*Rigid->AddToSimulation);
        }
    }

    if (OutProjection)
    {
        *OutProjection = std::move(Projection);
    }

    return Actor;
}
