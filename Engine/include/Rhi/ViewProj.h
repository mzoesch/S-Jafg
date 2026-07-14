// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/Objects.h"

namespace Jafg::UBO
{

struct ViewProj final: rhi::ubo_template<ViewProj, vk::ShaderStageFlagBits::eVertex>
{
    explicit constexpr ViewProj(LMat4F Mat) noexcept: Mat{std::move(Mat)} {}

    LMat4F Mat;
};
static_assert(rhi::ubo<ViewProj>);

} /* ~Namespace Jafg::UBO */
