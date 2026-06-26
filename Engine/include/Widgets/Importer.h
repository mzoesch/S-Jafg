// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Asset.h"
#include "Nodes/UserWidget.h"
#include "Nodes/Viewport.h"
#include "Importer.generated.h"

namespace Jafg
{

class WText;
class WEditableTextButton;

ENGINE_API void CreateImporter(LViewport& Viewport, finder::path const& Path, Detail::EAsset Asset);

namespace Detail
{

//# Do not create the importer manually, use #CreateImporter instead.
DECLARE_JAFG_WIDGET()
class ENGINE_API WImporter final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_DYNAMIC_CTOR(WImporter)
    template<typename TCxxClass>
    explicit WImporter(TNodeStaticInit<TCxxClass> const& Init, finder::path Path, EAsset Asset) noexcept
        : Super{Init}, Path{std::move(Path)}, Asset{Asset}
    {
    }

public:

    virtual void Construct() override;

private:

    finder::path Path;
    finder::path BasePath;
    EAsset Asset;

    WEditableTextButton* Target{};
    WEditableTextButton* Format{};
    WEditableTextButton* MipLevels{};
    WEditableTextButton* MaxSampleCount{};

    WText* ErrorMessage{};
};

} /* ~Namespace Detail */

} /* ~Namespace Jafg */
