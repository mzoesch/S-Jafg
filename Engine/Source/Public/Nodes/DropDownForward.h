// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Texture2.h"
#include "Nodes/GenericTabInfos.h"

namespace Jafg
{

struct LDropDownNodeOption;
struct LDropDownNodeSeparator;
struct LDropDownNodeSubMenu;
// struct LDropDownNodeDeferredSubMenu;

typedef std::variant<
      LDropDownNodeSubMenu
    , LDropDownNodeOption
    , LDropDownNodeSeparator
    > LDropDownNode;

struct LDropDownNodeOption final
{
    LTabSelectorCreateInfo Selector;
    // TODO: Add shortcut hint
    TCopyableFunction<algo::reply()> OnAction;
};

struct LDropDownNodeSeparator final
{
    LString DisplayName;
    LOptionalTexture2Ref Icon;
    u32 Thickness{ 1 };
};

struct LDropDownNodeSubMenu final
{
    LString DisplayName;
    LOptionalTexture2Ref Icon;
    TArray<LDropDownNode> Children;
};

// struct LDropDownNodeDeferredSubMenu final : public Detail::LDropDownNodeBase
// {
// };

struct LDropDownRoot final
{
    TArray<LDropDownNodeSubMenu> Nodes;
};

} /* ~Namespace Jafg */
