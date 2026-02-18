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

std::shared_ptr<Jafg::LStaticMesh> Jafg::JMeshSubsystem::GetMesh(
      LPath const* Path
    , LString const& Ident
    , LStaticMesh::ELoadBehavior LoadBehavior
    , LStaticMesh::EUploadHostMemoryBehavior HostMemoryBehavior
    ) const
{
    check( Ident.empty() == false )

    if (auto const& It{this->Meshes.find(Ident)}; It != this->Meshes.end())
    {
        if (LoadBehavior == LStaticMesh::ELoadBehavior::Load)
        {
            if (It->second->VertexBuffer.GetBuffer())
            {
                LOG_FATAL(LogRhi, "Mesh [{}] is set to only load but is already on device.", Ident)
            }
            It->second->ReloadModel(LoadBehavior, HostMemoryBehavior);
        }
        else if (LoadBehavior == LStaticMesh::ELoadBehavior::LoadToDevice)
        {
            if (It->second->Vertices.empty())
            {
                It->second->ReloadModel(LoadBehavior, HostMemoryBehavior);
            }
            else
            {
                It->second->LoadToDevice(HostMemoryBehavior);
            }
        }

        return It->second;
    }

    if (Path == nullptr)
    {
        return {};
    }

    this->Meshes[Ident] = std::make_unique<LStaticMesh>(*Path, LoadBehavior, HostMemoryBehavior);
    auto& Reference{this->Meshes[Ident]};
    return Reference;
}
