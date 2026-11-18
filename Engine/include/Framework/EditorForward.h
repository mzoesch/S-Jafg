// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#if JAFG_WITH_EDITOR

namespace Jafg
{

enum struct EWorldRenderBits: u32 /* Stable across slang. */
{
    Identity = 0x0 << 0,

    //# Lighting.
    ShowDirectionLights = 0x1 << 0,
    ShowPointLights     = 0x1 << 1,
    ShowSpotLights      = 0x1 << 2,
    ShowRectLights      = 0x1 << 3,
    ShowIbl             = 0x1 << 4,

    //# Texture visualization.
    ShowAlbedo          = 0x1 <<  5,
    ShowNormal          = 0x1 <<  6,
    ShowOcclusion       = 0x1 <<  7,
    ShowRoughness       = 0x1 <<  8,
    ShowMetallic        = 0x1 <<  9,
    ShowEmissive        = 0x1 << 10,

    //# Screen space ambient occlusion.
    ShowSsao            = 0x1 << 11,

    //# Shadows of direct lights.
    ShowShadows         = 0x1 << 12,

    All = std::numeric_limits<u32>::max(),
};
ENUM_STRUCT_FLAGS(EWorldRenderBits, EWorldRenderFlags)

SERDE_DECLARE_TRIVIAL_ENUM(EEditorVisualization, u32,
    //# Show as it would be in shipping.
    Rendered,
    //# Show normal lit world but skip complex processing.
    Preview,
    //# No lighting, post-processing, etc. at all.
    Unlit,
    //# Show lighting only, no materials.
    LightingOnly,
    //# Show only wireframe.
    Wireframe
    )

NODISCARD inline constexpr EWorldRenderFlags EditorVisualizationPresetToWorldRenderPreferences(EEditorVisualization V)
{
    switch (V)
    {
    case EEditorVisualization::Rendered:
    case EEditorVisualization::Preview:
    {
        return EWorldRenderBits::All;
    }
    case EEditorVisualization::Unlit:
    {
        return EWorldRenderBits::ShowAlbedo
             | EWorldRenderBits::ShowNormal
             | EWorldRenderBits::ShowOcclusion
             | EWorldRenderBits::ShowRoughness
             | EWorldRenderBits::ShowMetallic
             | EWorldRenderBits::ShowEmissive
             ;
    }
    case EEditorVisualization::LightingOnly:
    {
        return EWorldRenderBits::ShowDirectionLights
             | EWorldRenderBits::ShowPointLights
             | EWorldRenderBits::ShowSpotLights
             | EWorldRenderBits::ShowRectLights
             | EWorldRenderBits::ShowSsao
             | EWorldRenderBits::ShowShadows
             ;
    }
    case EEditorVisualization::Wireframe:
    {
        return EditorVisualizationPresetToWorldRenderPreferences(EEditorVisualization::Unlit);
    }
    default:
    {
        LOG_FATAL(LogWidgets, "[{}]: Unknown preset.", static_cast<std::underlying_type_t<EEditorVisualization>>(V))
    }
    }
}

} /* ~Namespace Jafg */

JAFG_PREF_OF(Jafg::EEditorVisualization)

#endif /* JAFG_WITH_EDITOR */
