// Copyright mzoesch. All rights reserved.

//
// The default perspective camera UBO used by Jafg.
// @see Bindings/PerspectiveCamera.jafg.slang
//
struct UBO_PerspectiveCamera
{
    float4x4 view;
    float4x4 proj;
};
