// Copyright mzoesch. All rights reserved.

#pragma once

#include <deque>
#include "Engine/Jxx.h"
#include "Rhi/StaticMesh.h"
#include "Rhi/MeshScene.h"
#include "Rhi/SkeletalMesh.h"
#include "Rhi/Animation.h"
#include "Framework/MeshSceneProjection.h"
#include "Framework/EngineSubsystem.h"
#include "MeshSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API JMeshSubsystem final : public JEngineSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JMeshSubsystem)

public:

    //# Removes all loaded meshes (static and skeletal) and animations that are not referenced anymore.
    void PurgeUnused() noexcept;

    FORCEINLINE auto GetLoadedMeshCount() const noexcept { return this->Meshes.size(); }
    FORCEINLINE constexpr auto const& GetMeshes() const noexcept { return this->Meshes; }

    //#
    //# Loads the only mesh of a glTF file (.gltf or .glb), e.g., the default cube. Fatal if the file has more
    //# than one mesh; use #LoadScene or #LoadAndProjectScene for those.
    //#
    //# The first load of a file goes through #LoadScene. Then only the mesh of the file is remembered, so later
    //# loads of the same file do not touch the file anymore (unless the mesh has to be brought to a new state).
    //#
    //# Node transforms are ignored. The mesh is returned in its own space.
    //#
    //# @param Path Relative to the content dir of the namespace.
    //#
    LStaticMeshRef LoadMesh(LPluginInfo Namespace, LStringView Path, EStaticMeshState State = EStaticMeshStateBits::Device, std::optional<LStaticMeshCreateInfo> Info = {}) const;

    //#
    //# Loads a scene of a glTF file (.gltf or .glb). The file is read only once: Every mesh the scene references is
    //# allocated as its own static mesh (or reused if already allocated) and brought to the requested state from
    //# the already parsed file.
    //#
    //# Meshes that fail to load are logged and left out (their nodes are kept, but without a mesh).
    //#
    //# @param Path Relative to the content dir of the namespace.
    //# @param SceneIdx Empty: The default scene of the file. If the file declares none, the first scene. If the
    //#                 file has no scenes at all, all nodes without a parent.
    //# @param State The state every mesh of the scene is brought to. #EStaticMeshStateBits::None only allocates
    //#              the meshes. They then read the file on their own once they are brought to a state.
    //#
    std::expected<LMeshScene, LStaticMesh::EResult> LoadScene(
          LPluginInfo Namespace
        , LStringView Path
        , std::optional<u32> SceneIdx = {}
        , EStaticMeshState State = EStaticMeshStateBits::Device
        , std::optional<LStaticMeshCreateInfo> Info = {}
        ) const;

    //#
    //# #LoadScene and then #ProjectMeshScene below the given component. Fatal if the scene cannot be loaded.
    //# @param Parent Where the scene is projected to. E.g., the root of an actor.
    //#
    LMeshSceneProjection LoadAndProjectScene(
          LPluginInfo Namespace
        , LStringView Path
        , ASceneComponent& Parent
        , LMeshSceneProjectionInfo const& Projection = {}
        , EStaticMeshState State = EStaticMeshStateBits::Device
        , std::optional<u32> SceneIdx = {}
        , std::optional<LStaticMeshCreateInfo> Info = {}
        ) const;

    //#
    //# Loads the skinned mesh of a glTF file with its skeleton (@see #Gltf::PullSkeletalMesh) and uploads it to the
    //# device. Cached: Later loads of the same file return the same mesh.
    //#
    //# @param Path Relative to the content dir of the namespace.
    //# @return Null if the file has no usable skinned mesh. Logged.
    //#
    LSkeletalMeshRef LoadSkeletalMesh(LPluginInfo Namespace, LStringView Path) const;

    //#
    //# Loads all animations of a glTF file. Cached: Later loads of the same file return the same library.
    //#
    //# @param Path Relative to the content dir of the namespace.
    //# @return Null if the file has no usable animation. Logged.
    //#
    LAnimationLibraryRef LoadAnimations(LPluginInfo Namespace, LStringView Path) const;

    //#
    //# Frees buffers once the device is done with them: After every frame that is in flight now has finished. For
    //# buffers that were used for drawing until now (e.g., of a component that is torn down).
    //#
    void DeferRelease(TArray<rhi::mapped_device_buffer> Buffers) const;
    //# Frees the buffers of #DeferRelease that are due. Cheap if there are none. Called by their users once per frame.
    void ReleaseDeferred() const noexcept;
    //# Frees all buffers of #DeferRelease at once. Only while the device is idle (e.g., when the frontend is torn down).
    void ReleaseAllDeferred() const noexcept;

private:

    //# Lexically normal and with forward slashes. So that many spellings of a path share the same cache entries.
    NODISCARD static LString NormalizePath(LStringView Path) noexcept;

    //#
    //# Allocate the requested resource (or reuse it). If it has to be brought to a state, the source file is read
    //# for this mesh only.
    //# @param Source Its path must be normalized.
    //#
    LStaticMeshRef FromSource(LStaticMeshSource const& Source, EStaticMeshState State, std::optional<LStaticMeshCreateInfo> const& Info) const;
    //# @param Source Its path must be normalized. Its namespace is only dereferenced if the mesh has to be allocated.
    NODISCARD std::shared_ptr<LStaticMesh>& FindOrAllocate(LStaticMeshSource const& Source, std::optional<LStaticMeshCreateInfo> const& Info) const;

    mutable std::unordered_map<LStaticMeshSource, std::shared_ptr<LStaticMesh>> Meshes;
    //#
    //# Per namespace: Files loaded with #LoadMesh (normalized) to the index of their only mesh. Indices instead of
    //# mesh refs, so that #PurgeUnused still sees when a mesh is not used anymore.
    //#
    mutable std::unordered_map<LPluginInfo, algo::transparent_unordered_string_map<u32>> SingleMeshIndices;

    //# Per namespace: Normalized paths to what was loaded from them. Failed loads are not remembered.
    mutable std::unordered_map<LPluginInfo, algo::transparent_unordered_string_map<LSkeletalMeshRef>> SkeletalMeshes;
    mutable std::unordered_map<LPluginInfo, algo::transparent_unordered_string_map<std::shared_ptr<LAnimationLibrary>>> AnimationLibraries;

    struct LDeferredRelease final
    {
        //# The engine frame from which on the device no longer uses the buffers.
        u64 Frame;
        TArray<rhi::mapped_device_buffer> Buffers;
    };
    mutable std::deque<LDeferredRelease> DeferredReleases;
};

} /* ~Namespace Jafg */
