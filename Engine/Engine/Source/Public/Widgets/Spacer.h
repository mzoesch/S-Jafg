// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetNode.h"
#include "Spacer.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API WSpacer final : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WSpacer)

public:

    virtual void UpdateDesiredSize() const override;

    FORCEINLINE WSpacer& SetSize(const LVector2& InSize)
    {
        this->Size = InSize;
        return *this;
    }

    FORCEINLINE WSpacer& SetHeight(const float InHeight)
    {
        this->Size.Y = InHeight;
        return *this;
    }

    FORCEINLINE WSpacer& SetWidth(const float InWidth)
    {
        this->Size.X = InWidth;
        return *this;
    }

private:

    LVector2 Size = LVector2::Zero();
};

} /* ~Namespace Jafg */
