// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "Nodes/UserWidget.h"
#include "Rhi/Texture2.h"

namespace Jafg
{

struct LTabSelectorCreateInfo final
{
    //# The display name of the tab.
    LString DisplayName;
    //# The icons of the tab.
    LOptionalTexture2Ref Icon;
};
template<typename T>
concept CTabSelectorCandidate = requires
{
    { T::TabSelectorCreateInfo() } -> std::same_as<LTabSelectorCreateInfo>;
};

#define JAFG_DEFAULT_TAB_SELECTOR_CREATE_INFO(InDisplayName, InIcon) \
    inline static LTabSelectorCreateInfo TabSelectorCreateInfo() \
    { \
        return { \
            .DisplayName = InDisplayName, \
            .Icon = InIcon, \
            }; \
    }

//# Use this struct to describe a tap you want to add to #WTabOverlay.
struct LTabCreateInfo final
{
    LTabSelectorCreateInfo Selector;
    std::variant<TSubclassOf<WUserWidget>, TJxxUnique<WUserWidget>> Panel;

    decltype(auto) operator+(LTabCreateInfo&& Sibling) && noexcept
    {
        this->_Siblings.emplace_back(std::move(Sibling));
        return std::move(*this);
    }

    /* Jafg internal member. Do not use. */
    TArray<LTabCreateInfo> _Siblings;
};

template<typename T>
concept CTabCandidate = requires
{
    { T::TabCreateInfo() } -> std::same_as<LTabCreateInfo>;
};

#define JAFG_DEFAULT_TAB_CREATE_INFO() \
    inline static ::Jafg::LTabCreateInfo TabCreateInfo() noexcept \
    { \
        return { \
            .Selector = TabSelectorCreateInfo(), \
            .Panel = StaticClass(), \
            }; \
    }

#define JAFG_DEFAULT_TAB_CANDIDATE(InDisplayName, InIcon) \
    JAFG_DEFAULT_TAB_SELECTOR_CREATE_INFO(InDisplayName, InIcon) \
    JAFG_DEFAULT_TAB_CREATE_INFO()

} /* ~Namespace Jafg */
