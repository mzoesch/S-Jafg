// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Asset.h"
#include "Framework/EngineSubsystem.h"
#include "Rhi/TextureView.h"
#include "AssetDiscoverer.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JAssetDiscoverer final : public JEngineSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JAssetDiscoverer)

    void Initialize(LSubsystemCollection& Collection) override;

public:

    inline static constexpr u64 LatestVersion{ 1uz };

    NODISCARD FORCEINLINE auto const& GetHeaders() const noexcept { return this->CachedHeaders; }
    NODISCARD FORCEINLINE Detail::LAssetHeader const& GetHeader(LPath const& Path) const noexcept
    {
        return this->CachedHeaders.at(Path);
    }

    void PushAsset(LPath const& Path, LTextureView const& View);

    LTextureView PullAssetTextureView(LPath const& Path) const;

private:

    std::unordered_map<LPath, Detail::LAssetHeader> CachedHeaders;
    void Recache();
    void Recache(LPath const& Path);
};

} /* ~Namespace Jafg */
