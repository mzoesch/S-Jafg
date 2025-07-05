// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/ParentBaseFactory.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryRegion : public TWidgetFactoryParentBase<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryParentBase)

    FORCEINLINE TFactoryRetTy& Brush(const LRegionBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Type(const ERegionBrush::Type InType) { this->This()->SetType(InType); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Tint(const LColor& InTint) { this->This()->SetTint(InTint); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Texture(const LTexture2* InTexture) { this->This()->SetTexture(InTexture); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Image(const LImage& InImage) { this->This()->SetImage(InImage); return this->Self(); }
    FORCEINLINE TFactoryRetTy& ImageTint(const LColor& InColor) { this->This()->SetImageTint(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& ImageScale(const f32 InScale) { this->This()->SetImageScale(InScale); return this->Self(); }
    FORCEINLINE TFactoryRetTy& ImageBehavior(const EImageBehavior::Type InType) { this->This()->SetImageBehavior(InType); return this->Self(); }
    FORCEINLINE TFactoryRetTy& ImageOobm(const EImageOobm::Type InType) { this->This()->SetImageOobm(InType); return this->Self(); }
    FORCEINLINE TFactoryRetTy& ImagePadding(const f32 InPadding) { this->This()->SetImagePadding(InPadding); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineThickness(const f32 InOutlineThickness) { this->This()->SetOutlineThickness(InOutlineThickness); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineRadii(const LVector4& InOutlineRadii) { this->This()->SetOutlineRadii(InOutlineRadii); return this->Self(); }
    FORCEINLINE TFactoryRetTy& OutlineTint(const LColor& InOutlineTint) { this->This()->SetOutlineTint(InOutlineTint); return this->Self(); }
};

} /* ~Namespace Jafg */
