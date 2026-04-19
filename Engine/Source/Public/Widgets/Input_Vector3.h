// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/HButton.h"
#include "Input_Vector3.generated.h"

namespace Jafg
{

struct LFactoryInput_Vector3;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryInput_Vector3)
class ENGINE_API WInput_Vector3 : public WHButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WInput_Vector3) noexcept
    {
        this->Padding = 5.0f;
        this->Style.ChainEverywhere<
            &LRegionBrush::OutlineThickness,
            &LRegionBrush::Tint
            >(1.0f, Colors::Transparent);
    }

public:

    virtual void Construct() override;

    void SetDisplayName(const LString& InDisplayName);
    void SetDisplayName(LString&& InDisplayName);
    void SetValue(const LVec3F& InValue);

    FORCEINLINE const LString& GetDisplayName() const noexcept { return this->DisplayName; }
    FORCEINLINE const LVec3F& GetValue() const noexcept { return this->Value; }

private:

    LString DisplayName;
    LVec3F Value;
};

struct LFactoryInput_Vector3 : NODE_FACTORY_PARENT(WInput_Vector3)
{
    NODE_FACTORY_BODY(WInput_Vector3)

    decltype(auto) DisplayName(this auto&& Self, LString const& InDisplayName)
     {
        NODE_FACTORY_SELF().SetDisplayName(InDisplayName);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisplayName(this auto&& Self, LString&& InDisplayName)
    {
        NODE_FACTORY_SELF().SetDisplayName(std::move(InDisplayName));
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Value(this auto&& Self, LVec3F const& InValue)
    {
        NODE_FACTORY_SELF().SetValue(InValue);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
