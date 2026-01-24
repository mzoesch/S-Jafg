// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/RegionFactory.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryHRegion : public TWidgetFactoryRegion<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryRegion)

    FORCEINLINE TFactoryRetTy& HSpace(const f32 InHSpace) { this->This()->SetHSpace(InHSpace); return this->Self(); }
};

} /* ~Namespace Jafg */
