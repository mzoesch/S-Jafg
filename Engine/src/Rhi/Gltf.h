// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/StaticMesh.h"

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
    #include "tiny_gltf_v3.h"
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */
#if JAFG_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* JAFG_WITH_GCC */

//#
//# Engine private. Reading of glTF files (.gltf and .glb). Only geometry and the node hierarchy are read. Materials,
//# textures, cameras, lights, skins and animations are ignored.
//#
namespace Jafg::Gltf
{

//# A parsed glTF file. Owns all memory of the model.
struct LModel final
{
    LModel() noexcept { this->Native.default_scene = -1; }
    PROHIBIT_REALLOC_OF_ANY_FORM(LModel)
    ~LModel() { tg3_model_free(&this->Native); }

    NODISCARD FORCEINLINE tg3_model const& operator*() const noexcept { return this->Native; }
    NODISCARD FORCEINLINE tg3_model const* operator->() const noexcept { return &this->Native; }

    tg3_model Native{};
};

//#
//# Parses a .gltf or .glb file. External buffers are resolved relative to the directory of the file and may not
//# leave it. Images are not decoded.
//#
NODISCARD LStaticMesh::EResult Parse(LPath const& Path, LModel& Out);

//#
//# Pulls the host data of one mesh of a parsed model.
//# @param Path For diagnostics only.
//# @param Out Gets exactly one mesh on success.
//#
NODISCARD LStaticMesh::EResult PullMesh(LModel const& Model, LPath const& Path, u32 MeshIdx, TArray<LStaticMesh::HostMesh>& Out);

//#
//# The transform of a node relative to its parent node (or the scene).
//# A matrix is decomposed. Shear cannot be represented and is lost.
//# @param Path For diagnostics only.
//#
NODISCARD LWorldTrans GetLocalTransform(tg3_node const& Node, LPath const& Path) noexcept;

NODISCARD FORCEINLINE LString ToString(tg3_str const& Str) noexcept
{
    return Str.data ? LString{Str.data, Str.len} : LString{};
}

} /* ~Namespace Jafg::Gltf */
