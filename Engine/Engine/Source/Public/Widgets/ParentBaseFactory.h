// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Node.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryParentBase : public TWidgetFactory<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactory)

    FORCEINLINE TFactoryRetTy& Padding(const LPadding&  InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Padding(const LPadding&& InPadding) { this->This()->SetPadding(InPadding); return this->Self(); }

    FORCEINLINE TFactoryRetTy& AddChild(LWidgetFactory* InChild);
    FORCEINLINE TFactoryRetTy& operator[](LWidgetFactory& InChild) { return this->AddChild(&InChild); }
    FORCEINLINE TFactoryRetTy& operator[](LWidgetFactory* InChild) { return this->AddChild(InChild); }
};

} /* ~Namespace Jafg */
