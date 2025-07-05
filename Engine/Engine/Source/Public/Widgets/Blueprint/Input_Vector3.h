// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/HButton.h"
#include "Input_Vector3.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryInput_Vector3 : public TWidgetFactoryHButton<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryHRegion)

    FORCEINLINE TFactoryRetTy& DisplayName(const LString& InDisplayName) { this->This()->SetDisplayName(InDisplayName); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisplayName(LString&& InDisplayName) { this->This()->SetDisplayName(std::move(InDisplayName)); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Value(const LVector3& InValue) { this->This()->SetValue(InValue); return this->Self(); }
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryInput_Vector3)
class ENGINE_API WInput_Vector3 : public WHButton
{
    GENERATED_CLASS_BODY()

protected:

    explicit WInput_Vector3(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Construct() override;

    void SetDisplayName(const LString& InDisplayName);
    void SetDisplayName(LString&& InDisplayName);
    void SetValue(const LVector3& InValue);

    FORCEINLINE const LString& GetDisplayName() const noexcept { return this->DisplayName; }
    FORCEINLINE const LVector3& GetValue() const noexcept { return this->Value; }

private:

    LString DisplayName;
    LVector3 Value;
};

} /* ~Namespace Jafg */
