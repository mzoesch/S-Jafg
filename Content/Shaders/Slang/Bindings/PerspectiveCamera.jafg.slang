// Copyright mzoesch. All rights reserved.
#include "Ubos/PerspectiveCamera.jafg.slang"

//
// The default perspective camera bindings used by Jafg.
// @see Jafg::LRenderInfo::PerspectiveCameraDescriptorSet
//
[[vk::binding(0, 0)]] ConstantBuffer<UBO_PerspectiveCamera> camera;
