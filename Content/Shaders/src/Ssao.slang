// Copyright mzoesch. All rights reserved.

///
/// Screen space ambient occlusion: Estimates from the depth of the scene how much of the hemisphere above each pixel
/// is blocked by nearby geometry. The result is noisy on purpose and has to be blurred, see SsaoBlur.slang.
///
/// All positions are in a view space that is aligned with the screen: x to the right and y down like the pixel
/// coordinates, and z into the screen (the linear depth).
///

#include "Jafg.slang"
#include "Jafg/Ssao.slang"

[[CxxName("Jafg::PC::Ssao")]]
struct PushConstants
{
    float2 viewport_size;
    /// Tangent of half the horizontal (x) and vertical (y) field of view.
    float2 tan_half_fov;
    /// World space radius (m) in which geometry occludes.
    float radius;
    /// Occluders have to rise above the surface by this sine of an angle. Hides the limited depth precision and
    /// the faceting of low poly meshes.
    float bias;
    float intensity;
    /// Contrast of the result.
    float power;
};

[[vk::binding(0, 0),UpdateFrequency(Rarely)]] uniform Texture2D<float> depth_texture;

[[vk::push_constant]] PushConstants ssao;

static const int   SAMPLE_COUNT = 12;
/// Upper limit for the radius in pixels. Keeps the texture fetches of close up geometry cache friendly.
static const float MAX_PIXEL_RADIUS = 64.0;
static const float GOLDEN_ANGLE = 2.39996323;

float load_depth(int2 pixel)
{
    return Jafg::ssao_load(depth_texture, pixel, ssao.viewport_size);
}

float3 view_pos(int2 pixel, float depth)
{
    float2 ndc = (float2(pixel) + 0.5) / ssao.viewport_size * 2.0 - 1.0;
    return float3(ndc * ssao.tan_half_fov * depth, depth);
}

float3 view_pos(int2 pixel)
{
    return view_pos(pixel, load_depth(pixel));
}

///
/// Reconstructs the normal from the depth. Per axis, the neighbor with the smaller depth difference is used so that
/// the normal does not get bent at depth discontinuities.
///
float3 view_normal(int2 pixel, float3 center)
{
    float3 l = view_pos(pixel - int2(1, 0));
    float3 r = view_pos(pixel + int2(1, 0));
    float3 u = view_pos(pixel - int2(0, 1));
    float3 d = view_pos(pixel + int2(0, 1));

    float3 dx = abs(l.z - center.z) < abs(r.z - center.z) ? center - l : r - center;
    float3 dy = abs(u.z - center.z) < abs(d.z - center.z) ? center - u : d - center;

    float3 n = cross(dx, dy);
    n *= rsqrt(max(dot(n, n), 1e-20));
    // Visible surfaces always face the camera, which is the origin.
    return dot(n, center) > 0.0 ? -n : n;
}

/// [Jimenez 2014, "Next Generation Post Processing in Call of Duty: Advanced Warfare"]
float interleaved_gradient_noise(float2 pixel)
{
    return frac(52.9829189 * frac(dot(pixel, float2(0.06711056, 0.00583715))));
}

[[shader("vertex")]]
Jafg::SsaoFRInput vertMain(uint vertex_id: SV_VertexID)
{
    return Jafg::ssao_fullscreen_triangle(vertex_id);
}

[[shader("fragment")]]
float1 fragMain(Jafg::SsaoFRInput input): SV_TARGET
{
    int2 pixel = int2(input.pos.xy);

    float depth = load_depth(pixel);
    if (depth >= Jafg::SSAO_BACKGROUND_DEPTH)
    {
        return 1.0;
    }

    // The radius projected onto the screen.
    float pixel_radius = ssao.radius * 0.5 * ssao.viewport_size.y / (ssao.tan_half_fov.y * depth);
    if (pixel_radius < 1.0)
    {
        return 1.0;
    }
    // If the radius is limited on the screen, the radius in the world has to shrink with it.
    float limited_pixel_radius = min(pixel_radius, MAX_PIXEL_RADIUS);
    float radius = ssao.radius * limited_pixel_radius / pixel_radius;

    float3 p = view_pos(pixel, depth);
    float3 n = view_normal(pixel, p);

    float noise = interleaved_gradient_noise(input.pos.xy);

    float occlusion = 0.0;
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        // Spiral that covers the disc evenly. The noise rotates it per pixel.
        float angle = (float(i) + noise) * GOLDEN_ANGLE + noise * 2.0 * PI;
        float dist = sqrt((float(i) + 0.5) / float(SAMPLE_COUNT)) * limited_pixel_radius;
        int2 sample_pixel = pixel + int2(round(float2(cos(angle), sin(angle)) * dist));

        float3 v = view_pos(sample_pixel) - p;
        float dist_sqr = dot(v, v);

        // How far the sample rises above the surface as the sine of the elevation angle.
        float elevation = dot(v, n) * rsqrt(max(dist_sqr, 1e-12));
        // Geometry further away than the radius does not occlude. This also rejects the background.
        float range = saturate(1.0 - dist_sqr / (radius * radius));

        occlusion += saturate(elevation - ssao.bias) * range;
    }
    occlusion /= float(SAMPLE_COUNT);

    return pow(saturate(1.0 - ssao.intensity * occlusion), ssao.power);
}
