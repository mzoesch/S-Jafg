// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/UniformBufferObjects.h"

namespace Jafg::UBO
{

struct ViewProj final : public TUbo<ViewProj, 0, vk::ShaderStageFlagBits::eVertex>
{
    LMat4F Mat;
};
static_assert(CUniformBufferObject<ViewProj>);

} /* ~Namespace Jafg::UBO */
