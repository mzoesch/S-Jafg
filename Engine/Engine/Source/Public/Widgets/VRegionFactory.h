// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/RegionFactory.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryVRegion : public TWidgetFactoryRegion<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryRegion)

    FORCEINLINE TFactoryRetTy& VSpace(const f32 InVSpace) { this->This()->SetVSpace(InVSpace); return this->Self(); }
};

} /* ~Namespace Jafg */
