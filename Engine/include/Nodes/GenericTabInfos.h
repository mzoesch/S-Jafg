// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "Nodes/UserWidget.h"
#include "Rhi/Texture2.h"
#include "GenericTabInfos.generated.h"

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
    std::optional<LTabSelectorCreateInfo> Selector;
    std::variant<TSubclassOf<WUserWidget>, TJxxUnique<WUserWidget>, LFactoryUserWidget> Panel;

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

DECLARE_JAFG_CLASS()
class ENGINE_API JTabSelectorInfoRetriever : public JNodeData
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JTabSelectorInfoRetriever)

public:

    std::optional<LTabSelectorCreateInfo> Selector;
};

#define JAFG_DEFAULT_TAB_CREATE_INFO() \
    inline static ::Jafg::LTabCreateInfo TabCreateInfo() noexcept \
    { \
        return { \
            .Selector = TabSelectorCreateInfo(), \
            .Panel = StaticClass(), \
            }; \
    }

#define JAFG_DEFAULT_TAB_DYNAMIC_RETRIEVER() \
    virtual ::algo::reply AddData(::Jafg::JNodeData& Data) override \
    { \
        if (auto* Retriever{Data.As<::Jafg::JTabSelectorInfoRetriever>()}) \
        { \
            check(!Retriever->Selector) \
            Retriever->Selector = Derived::TabSelectorCreateInfo(); \
            return ::algo::reply::handled(); \
        } \
        return {}; \
    }

#define JAFG_DEFAULT_TAB_CANDIDATE(InDisplayName, InIcon) \
    JAFG_DEFAULT_TAB_SELECTOR_CREATE_INFO(InDisplayName, InIcon) \
    JAFG_DEFAULT_TAB_CREATE_INFO() \
    JAFG_DEFAULT_TAB_DYNAMIC_RETRIEVER()

} /* ~Namespace Jafg */
