// Copyright mzoesch. All rights reserved.

#include "Framework/MeshSubsystem.h"

void Jafg::JMeshSubsystem::PurgeUnused() noexcept
{
    std::erase_if(this->Meshes, [](auto& E) -> bool
    {
        if (E.second.use_count() == 1)
        {
            LOG_VERBOSE(LogTextureSubsystem, "Purging unused mesh [{}].", E.first)
            return true;
        }

        return false;
    });

    // TODO: Do we want to rehash??
}

Jafg::LStaticMeshRef Jafg::JMeshSubsystem::FromFile(LPath const& Path, EStaticMeshState State /* = EStaticMeshStateBits::Device */, std::optional<LStaticMeshCreateInfo> Info /* = {} */) const
{
    if (auto const& It{this->Meshes.find(Path)}; It != this->Meshes.end())
    {
        LStaticMesh& Mesh{*It->second};
        if (Info)
        {
            Mesh.MaxConvexVertices = Info->MaxConvexVertices;
            Mesh.ConvexTolerance = Info->ConvexTolerance;
        }
        Mesh.BringToState(State);
        return It->second;
    }

    auto Result{std::make_shared<LStaticMesh>(Path)};
    this->Meshes[Path] = Result;
    if (Info)
    {
        Result->MaxConvexVertices = Info->MaxConvexVertices;
        Result->ConvexTolerance = Info->ConvexTolerance;
    }
    Result->BringToState(State);
    return Result;
}
