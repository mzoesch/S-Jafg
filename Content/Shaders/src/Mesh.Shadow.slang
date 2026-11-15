// Copyright mzoesch. All rights reserved.

///
/// Shadow pass: Writes only the depth of the scene as seen from a light into its tile of the shadow atlas.
/// The pipeline has no color attachment. @see Engine/include/Rhi/Shadow.h
///

#include "Jafg.slang"
#include "Jafg/MVP.slang"
#include "Jafg/StaticMeshVertex.slang"

[[vk::push_constant]] MVP mvp;

[[shader("vertex")]]
float4 vertMain(Jafg::StaticMeshVertex input): SV_POSITION
{
    return mul(mvp.proj, mul(mvp.view, mul(mvp.model, float4(input.pos, 1.0))));
}

/// Nothing to shade. Present so that the push constant range covers the same stages as on the host (see PC::MVP).
[[shader("fragment")]]
void fragMain()
{
}
