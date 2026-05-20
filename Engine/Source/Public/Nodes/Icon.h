// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"

namespace Jafg
{

struct LIconBrush
{
    inline static constexpr auto DefaultInwardsPadding{ 4_spt };
    inline static constexpr auto DefaultMinIconSize{ 20_spt };

    enum struct Align{ Left, Center, Right, };

    u32 Scale{ 1 };
    bool bAlwaysPad{};
    LNodeSize1 InwardsPadding{ 4_spt };
    LNodeSize1 MinIconSize{ 20_spt };
    Align Alignment{ Align::Center };
    LColor Tint{ Colors::White };
};

} /* ~Namespace Jafg */
