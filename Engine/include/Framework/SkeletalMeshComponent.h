// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "Rhi/SkeletalMesh.h"
#include "Rhi/Animation.h"
#include "Rhi/Material.h"
#include "Rhi/PhysicalRendering.h"
#include "Rhi/MaxFramesInFlight.h"
#include "SkeletalMeshComponent.generated.h"

namespace Jafg
{

//# What a body draws while a controller sees through a first-person eye of its actor (@see #AEyeComponent::bFirstPerson).
enum struct EFirstPersonBody: u8
{
    //# Everything. The eye is probably inside of the mesh.
    Full,
    //# Only the triangles that the first-person joints move (e.g., arms and hands). @see #SetFirstPersonJoints
    Partial,
    //# Nothing.
    Hidden,
};

//#
//# Draws a skeletal mesh in a pose and animates it with clips.
//#
//# Every tick, the animation player (@see #GetPlayer) is advanced and evaluated into a local pose. An optional pose
//# modifier changes that pose (e.g., the head follows the view), before it is resolved into model space. Drawing skins
//# the mesh on the host into a vertex buffer of this component, once per frame and only if the pose changed.
//#
//# Animation is cosmetic: Nothing that is simulated (movement, physics, netcode) should depend on the pose.
//#
//# The model space of the mesh is the local space of this component. E.g., a character whose root is at its feet and
//# whose mesh has its origin at the feet puts this component directly below the root.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API ASkeletalMeshComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(ASkeletalMeshComponent)
    {
        this->SetShouldRender(true);
        this->bTick = true;
    }

public:

    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;
    virtual void ParentTick(f32 Dt) override;
    //# Draws the mesh (skinned to the current pose) and then the child components.
    virtual void Render(LActorRenderInfo const& Info) const override;

    //# ---------- Mesh ----------

    //#
    //# The skinned mesh of a glTF file. @see #JMeshSubsystem::LoadSkeletalMesh
    //# @return Whether the mesh could be loaded. If not, the previous mesh is kept. Logged.
    //#
    bool SetMesh(LPluginInfo Namespace, LStringView Path);
    //# Shows a loaded mesh. Null shows nothing. The pose starts in the rest pose; clips keep playing.
    void SetMesh(LSkeletalMeshRef InMesh);
    NODISCARD FORCEINLINE bool IsMeshValid() const noexcept { return !!this->Mesh; }
    NODISCARD FORCEINLINE LSkeletalMeshRef const& GetMesh() const noexcept { return this->Mesh; }
    //# Null if there is no mesh.
    NODISCARD FORCEINLINE LSkeleton const* GetSkeleton() const noexcept { return this->Mesh ? this->Mesh->Skeleton.get() : nullptr; }

    void SetMaterialInstance(LMaterialInstanceRef Instance) noexcept;
    void SetPbrMaterialInstance(LPbrMaterialInstanceRef Instance) noexcept;

    //# ---------- Animation ----------

    //#
    //# Makes the clips of a glTF file available by their names. @see #JMeshSubsystem::LoadAnimations
    //# Clips of files that are added later hide clips with the same name of earlier files.
    //# @return The number of clips that were added. Zero if the file could not be loaded. Logged.
    //#
    std::size_t AddAnimations(LPluginInfo Namespace, LStringView Path);
    void AddAnimations(LAnimationLibraryRef Library);
    //# Null if no added file has a clip with this name.
    NODISCARD LAnimationClipRef FindClip(LStringView Name) const noexcept;
    NODISCARD FORCEINLINE bool HasClip(LStringView Name) const noexcept { return !!this->FindClip(Name); }

    //# Fades a clip in by its name. @return Whether the clip exists. Nothing changes if not.
    bool Play(LStringView Clip, LAnimationPlayParams const& Params = {});
    //# Fades a clip in. Null fades to the rest pose.
    void Play(LAnimationClipRef Clip, LAnimationPlayParams const& Params = {}) { this->Player.Play(std::move(Clip), Params); }
    //# Fades to the rest pose.
    void Stop(f32 BlendTime = 0.2f) { this->Player.Stop(BlendTime); }
    NODISCARD FORCEINLINE LAnimationPlayer& GetPlayer() noexcept { return this->Player; }
    NODISCARD FORCEINLINE LAnimationPlayer const& GetPlayer() const noexcept { return this->Player; }

    //#
    //# Changes the local pose after the clips were sampled and before it is resolved, every tick. E.g., to let the
    //# head follow the pitch of the view (@see #LSkeleton::RotateInModelSpace). Empty removes it.
    //#
    typedef TCopyableFunction<void(ASkeletalMeshComponent& Comp, std::span<LJointTransform> Pose)> LPoseModifier;
    void SetPoseModifier(LPoseModifier Modifier) { this->PoseModifier = std::move(Modifier); }

    //# Scales the time of all clips. Zero freezes the pose.
    CLASS_FIELD(EditorEditable)
    f32 PlayRate{ 1.0f };

    //# ---------- Pose ----------

    //# Of the last tick. Relative to the parent joints. Empty without a mesh.
    NODISCARD FORCEINLINE std::span<LJointTransform const> GetLocalPose() const noexcept { return this->LocalPose; }
    //# Of the last tick. In the space of this component. Empty without a mesh.
    NODISCARD FORCEINLINE std::span<LMat4F const> GetModelPose() const noexcept { return this->ModelPose; }
    NODISCARD std::optional<u32> FindJoint(LStringView Name) const noexcept;
    //# In world space, of the last tick. E.g., to attach a tool to a hand.
    NODISCARD LWorldTrans GetJointWorldTransform(u32 Joint) const noexcept;

    //# ---------- First person ----------

    //# @see #AEyeComponent::bFirstPerson
    EFirstPersonBody FirstPersonBody{ EFirstPersonBody::Hidden };
    //#
    //# The joints whose triangles are drawn in first person with #EFirstPersonBody::Partial: Every joint whose name
    //# starts with one of #NamePrefixes. A triangle is drawn if each of its vertices is moved at least half by them.
    //#
    void SetFirstPersonJoints(TArray<LString> NamePrefixes);

private:

    //# Sizes the pose to the mesh and resolves it from the player. Advances the player by #Dt first.
    void UpdatePose(f32 Dt);
    NODISCARD LAnimationClipBinding const& GetBinding(LAnimationClip const& Clip) const;
    void RebuildFirstPersonSubset();
    //# Hands the vertex buffers to the mesh subsystem, which frees them once the device no longer uses them.
    void ReleaseFrameBuffers() noexcept;
    //# Whether #Info is drawn for a controller that looks out of a first-person eye of this actor.
    NODISCARD bool IsSeenFromInside(LActorRenderInfo const& Info) const noexcept;
    //# @param Subset Null draws everything.
    void DrawSkinned(LActorRenderInfo const& Info, LSkeletalMesh::LSubset const* Subset) const;

    LSkeletalMeshRef Mesh;
    LMaterialInstanceRef MaterialInstance;
    LPbrMaterialInstanceRef PbrMaterialInstance;

    TArray<LAnimationLibraryRef> Libraries;
    //# Per clip, against the skeleton of #Mesh. Cleared when the mesh changes.
    mutable std::unordered_map<LAnimationClip const*, LAnimationClipBinding> Bindings;
    LAnimationPlayer Player;
    LPoseModifier PoseModifier;

    TArray<LJointTransform> LocalPose;
    TArray<LMat4F> ModelPose;
    TArray<LMat4F> SkinMatrices;
    //# Changes whenever the pose changes. The vertex buffers remember which pose they hold.
    u64 PoseVersion{};
    bool bPoseValid{};

    TArray<LString> FirstPersonJoints;
    LSkeletalMesh::LSubset const* FirstPersonSubset{};

    struct LFrameBuffer final
    {
        rhi::mapped_device_buffer Vertices;
        //# The #PoseVersion the vertices are skinned to.
        u64 SkinnedVersion{ ~0ull };
    };
    //# Per frame in flight. Allocated on the first draw.
    mutable rhi::frame_array<LFrameBuffer> FrameBuffers;
};

} /* ~Namespace Jafg */
