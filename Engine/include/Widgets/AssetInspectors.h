// Copyright mzoesch. All rights reserved.

#pragma once

#include "Containers/Finder2.h"
#include "Framework/Asset.h"
#include "Nodes/Viewport.h"
#include "Nodes/TabOverlay.h"
#include "AssetInspectors.generated.h"

namespace Jafg
{

class WTextureInspector;
struct LFactoryTextureInspector;

ENGINE_API void CreateTextureInspector(LTabOverlayPossibilities& Where, finder::path const& Path);

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextureInspector)
class ENGINE_API WTextureInspector : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTextureInspector(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
    }
    template<typename TCxxClass>
    explicit WTextureInspector(TNodeStaticInit<TCxxClass> const& Init, finder::path Path) noexcept
        : Super{Init}, Asset{std::move(Path)}
    {
    }

public:

    virtual void Construct() override;

private:

    finder::path Asset;
    LString Path;
    LString Format;
    LString Texture;
    LString MipLevels;
    LString MaxSampleCount;
};

struct LFactoryTextureInspector : NODE_FACTORY_PARENT(WTextureInspector)
{
    NODE_FACTORY_BODY(WTextureInspector)

    decltype(auto) Path(this auto&& Self, finder::path Path) noexcept
    {
        NODE_FACTORY_SELF().Path = std::move(Path);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
