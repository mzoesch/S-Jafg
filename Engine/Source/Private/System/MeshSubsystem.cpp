// Copyright mzoesch. All rights reserved.

#include "System/MeshSubsystem.h"

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

    return;
}

Jafg::LStaticMeshRef Jafg::JMeshSubsystem::FromFile(LPath const& Path, EStaticMeshState State /* = EStaticMeshStateBits::Device */) const
{
    if (auto const& It{this->Meshes.find(Path)}; It != this->Meshes.end())
    {
        LStaticMesh& Mesh{*It->second};

        if (State & EStaticMeshStateBits::Host || ((State & EStaticMeshStateBits::Device) && Mesh.IsOnDevice() == false))
        {
            if (Mesh.IsOnHost() == false)
            {
                auto Result{Mesh.LoadToHost()};
                jassert(Result == LStaticMesh::EResult::Success)
            }
        }
        if (State & EStaticMeshStateBits::Device)
        {
            if (Mesh.IsOnDevice() == false)
            {
                Mesh.LoadToDevice();
                if ((State & EStaticMeshStateBits::Host) == EStaticMeshStateBits::None)
                {
                    Mesh.FreeFromHost();
                }
            }
        }

        return It->second;
    }

    this->Meshes[Path] = std::make_shared<LStaticMesh>(Path, State);
    return this->Meshes[Path];
}
