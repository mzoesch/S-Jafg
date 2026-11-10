// Copyright mzoesch. All rights reserved.

///
/// Removes the noise of the screen space ambient occlusion with a blur that does not cross depth discontinuities,
/// so that the occlusion of an object does not bleed onto what is behind it.
///

#include "Jafg.slang"
#include "Jafg/Ssao.slang"

[[CxxName("Jafg::PC::SsaoBlur")]]
struct PushConstants
{
    float2 viewport_size;
    /// Relative depth difference at which a neighbor does not contribute anymore.
    float depth_threshold;
    float _pad0;
};

[[vk::binding(0, 0),UpdateFrequency(Rarely)]] uniform Texture2D<float> occlusion_texture;
[[vk::binding(1, 0),UpdateFrequency(Rarely)]] uniform Texture2D<float> depth_texture;

[[vk::push_constant]] PushConstants blur;

static const int BLUR_RADIUS = 2;

[[shader("vertex")]]
Jafg::SsaoFRInput vertMain(uint vertex_id: SV_VertexID)
{
    return Jafg::ssao_fullscreen_triangle(vertex_id);
}

[[shader("fragment")]]
float1 fragMain(Jafg::SsaoFRInput input): SV_TARGET
{
    int2 pixel = int2(input.pos.xy);

    float depth = Jafg::ssao_load(depth_texture, pixel, blur.viewport_size);
    if (depth >= Jafg::SSAO_BACKGROUND_DEPTH)
    {
        return 1.0;
    }

    float sum = 0.0;
    float weight_sum = 0.0;
    for (int y = -BLUR_RADIUS; y <= BLUR_RADIUS; ++y)
    {
        for (int x = -BLUR_RADIUS; x <= BLUR_RADIUS; ++x)
        {
            int2 neighbor = pixel + int2(x, y);
            float neighbor_depth = Jafg::ssao_load(depth_texture, neighbor, blur.viewport_size);

            // The center always has the full weight, so that the sum of the weights is never zero.
            float weight = saturate(1.0 - abs(neighbor_depth - depth) / (blur.depth_threshold * depth));
            sum += Jafg::ssao_load(occlusion_texture, neighbor, blur.viewport_size) * weight;
            weight_sum += weight;
        }
    }

    return sum / weight_sum;
}
