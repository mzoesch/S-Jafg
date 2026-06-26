// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/GenericTabInfos.h"
#include "EditorFinder.generated.h"

namespace Jafg
{

//# A widget that can inspect tags.
DECLARE_JAFG_WIDGET()
class ENGINE_API WFinder final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WFinder)

public:

    JAFG_DEFAULT_TAB_CANDIDATE("Finder", "Icons/Jafg.File")

    virtual void Construct() override;

private:

    LNodeSize2 NameSize{256_spt, 0.0f};
    LNodeSize2 SizeSize{86_spt, 0.0f};
    LNodeSize2 ModifiedSize{108_spt, 0.0f};
    f32 IndentSize{16};
    NODISCARD FORCEINLINE LPadding Indent2Padding(std::size_t Indent) const
    {
        return LPadding{ENodeSize::StaticPoints, this->IndentSize * static_cast<f32>(Indent), 0.0f, 0.0f, 0.0f};
    }
    NODISCARD FORCEINLINE f32 Padding2Indent(LPadding const& Padding) const
    {
        return Padding.Size.x / this->IndentSize;
    }

    struct LEntry final
    {
        TClassStorage<WNode> Node;
        bool bExtended{};
    };
    std::unordered_map<finder::path, LEntry> Meta;

    void ExpandDirty(finder::path const& Path);
    void CollapseDirty(finder::path const& Path);

    WParent* Container{};
    void Repopulate();
    void Populate(std::size_t& Counter, std::size_t& Where, std::size_t Indent, finder::path const& Path);
    void Populate(std::size_t& Counter, std::size_t& Where, std::size_t Indent, finder::directory_entry const& Entry);

    void OnExtendUpdate(finder::path const& Path, LEntry& Entry);
    void Repaint();
};
static_assert(CTabSelectorCandidate<WFinder>);
static_assert(CTabCandidate<WFinder>);

} /* ~Namespace Jafg */
