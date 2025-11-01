// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "User/Frontend/Hud/Crosshair.h"
#include "User/Frontend/Osd/PauseScreen.h"
#include "SvwFrontendSubsystem.generated.h"

namespace Jgc
{

DECLARE_JAFG_CLASS()
class JSvwFrontendSubsystem : public Jafg::JWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JSvwFrontendSubsystem)

    virtual bool ShouldCreateSubsystem(Jafg::LClassOuter const* Outer) const override;
    virtual void Initialize(Jafg::LSubsystemCollection& Collection) override;

public:

    FORCEINLINE Jafg::WCrosshair* GetCrosshair() noexcept { return this->Crosshair; }
    FORCEINLINE Jafg::WCrosshair const* GetCrosshair() const noexcept { return this->Crosshair; }

    FORCEINLINE Jafg::WPauseScreen* GetPauseScreen() noexcept { return this->PauseScreen; }
    FORCEINLINE Jafg::WPauseScreen const* GetPauseScreen() const noexcept { return this->PauseScreen; }

    CLASS_FIELD(Config)
    TSubclassOf<Jafg::WCrosshair> CrosshairClass{ DefaultInit };
    CLASS_FIELD(Config)
    TSubclassOf<Jafg::WPauseScreen> PauseScreenClass{ DefaultInit };

private:

    CDR_NULL_PTR(Jafg::WCrosshair*) Crosshair{ nullptr };
    CDR_NULL_PTR(Jafg::WPauseScreen*) PauseScreen{ nullptr };
};

} /* ~Namespace Jgc */
