// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/Objects.h"

namespace Jafg::UBO
{

struct SolidColorInput: rhi::ubo_template<SolidColorInput>
{
    LColor Color;
};
static_assert(rhi::ubo<SolidColorInput>);

} /* ~Namespace Jafg::UBO */
