// Copyright mzoesch. All rights reserved.

#pragma once

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

//#
//# High-level image behavior.
//#
namespace EImageBehavior
{

enum Type
{
    //#
    //# Scale the image to the size of the parent.
    //#
    Scale,

    //#
    //# Preserve the aspect ratio of the image.
    //#
    Aspect,
};

} /* ~Namespace EImageBehavior */

//#
//# The image out of bounds mode. How the image should behave if its UVs are going out of bounds.
//#
namespace EImageOobm
{

enum Type
{
    //#
    //# Wrap the image. The default behavior.
    //#
    Wrap        = 0,

    //#
    //# Clamp the image sides.
    //#
    Clamp       = 1,

    //#
    //# Discard the image channels.
    //#
    Discard     = 2,
};

} /* ~Namespace EImageOobm */

struct LRegionBrush
{
    //#
    //# The type of the region brush.
    //#
    ERegionBrush::Type Type{ ERegionBrush::None };

    //#
    //# The tint of the draw area from this region.
    //#
    Lal::LColor Tint{ Lal::LColor::White };

    //#
    //# An optional image to use as a background.
    //#
    LImage Image;

    //#
    //# The tint of the image.
    //#
    Lal::LColor ImageTint{ Lal::LColor::White };

    //#
    //# The scale of the image.
    //#
    f64 ImageScale{ 1.0 };

    //#
    //# How the #Image should behave.
    //#
    EImageBehavior::Type ImageBehavior{ EImageBehavior::Scale };

    //#
    //# The image out of bounds mode. @see #EImageOobm.
    //#
    EImageOobm::Type ImageOobm{ EImageOobm::Wrap };

    //#
    //# How much padding to apply to the image.
    //#
    f64 ImagePadding{ 0.0 };

    //#
    //# The radii to use for the edges. TL => TR => BR => BL.
    //#
    LVector4D Radii{ 4.0 };

    //#
    //# The thickness of the outline.
    //#
    f64 OutlineThickness{ 2.0 };

    //#
    //# The outline color to use.
    //#
    Lal::LColor OutlineTint{ Lal::LColor::White };
};

} /* ~Namespace Jafg */
