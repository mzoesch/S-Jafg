// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Rhi/Image.h"

namespace Jafg
{

//#
//# How the #LRegionBrush behaves at a high level.
//#
namespace ERegionBrush
{

    enum Type
    {
        //#
        //# Do not draw.
        //#
        None,

        //#
        //# Draw as a normal box.
        //#
        Box,

        //#
        //# Draw as a rounded box.
        //#
        RoundedBox,

        //#
        //# Draw as a box with an outline.
        //#
        OutlineBox,

        //#
        //# Draw as a box with a rounded outline.
        //#
        RoundedOutlineBox,
    };

} /* ~Namespace ERegionBrush */

struct LRegionBrush
{
    //#
    //# The type of the region brush.
    //#
    ERegionBrush::Type Type { ERegionBrush::None };

    //#
    //# The tint of the draw area from this region.
    //#
    LColor Tint { LColor::White };

    //#
    //# The color multiplier of the content from this region.
    //#
    LColor Color { LColor::White };

    //#
    //# The color multiplier of the background from this region.
    //#
    LColor BackgroundColor { LColor::White };

    //#
    //# An optional image to use as a background.
    //#
    LImage Image;

    //#
    //# The radii to use for the edges. TL => TR => BR => BL.
    //#
    LVector4 Radii { 4.0f };

    //#
    //# The thickness of the outline.
    //#
    f32 OutlineThickness { 2.0f };

    //#
    //# The outline color to use.
    //#
    LColor OutlineTint { LColor::White };
};


} /* ~Namespace Jafg */
