// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "Rhi/TextureView.h"
#include "TextureViewSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API JTextureViewSubsystem final : public JEngineSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JTextureViewSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override
    {
        Super::Initialize(Collection);
        this->ReloadTextureViews();
    }

    void ReloadTextureViews();

    FORCEINLINE TArray<LTextureView> const& GetTextureViews() const noexcept { return this->TextureViews; }

private:

    TArray<LTextureView> TextureViews;
};

} /* ~Namespace Jafg */
