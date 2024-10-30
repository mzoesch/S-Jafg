// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

class WUserWidget;

class RHI_API LViewport final
{
public:

    LViewport() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LViewport)
    ~LViewport() = default;

    void Initialize();
    void Tick();
    void Draw();
    void TearDown();

    void AddWidget(WUserWidget* Widget);
    void RemoveWidget(WUserWidget* Widget);

    auto ChangeDimensions(const LIntVector2& InDimensions) -> void;
    FORCEINLINE auto GetDimensions() const -> LIntVector2 { return this->Dimensions; }

private:

    LIntVector2            Dimensions;
    TdhArray<WUserWidget*> TopLevelWidgets;
};

} /* ~Namespace Jafg. */
