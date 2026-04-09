// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "Rhi/Texture2.h"

namespace Jafg
{

//# Use this struct to describe a tap you want to add to #WTabOverlay.
struct LTabOverlayElement
{
    struct CreateInfo
    {
        //# The display name of the tab.
        LString DisplayName;
        //#
        //# The icon of the tab.
        //# - std::monostate: No icon.
        //# - LTexture2Ref: The texture will be used (has to be valid).
        //# - LString: Texture view identifier (has to be valid).
        //#
        std::variant<std::monostate, LTexture2Ref, LString> Icon;
    };

    //# Either with the create-info struct or pass any valid factory.
    std::variant<CreateInfo, Detail::LNodeFactoryBase> Selector;
    //# The panel. Has to be valid.
    Detail::LNodeFactoryBase Panel;

    decltype(auto) operator+(LTabOverlayElement&& Sibling) && noexcept
    {
        this->_Siblings.emplace_back(std::move(Sibling));
        return std::move(*this);
    }

    /* Jafg internal member. Do not use. */
    TArray<LTabOverlayElement> _Siblings;
};

} /* ~Namespace Jafg */
