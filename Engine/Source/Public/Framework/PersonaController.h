// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Actor.h"
#include "Framework/Pawn.h"
#include "PersonaController.generated.h"

namespace Jafg
{

class LLocalEgo;
class LUserInput;
class WWorldNode;

DECLARE_JAFG_CLASS()
class ENGINE_API APersonaController : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(APersonaController)

public:

    friend WWorldNode;

    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    FORCEINLINE constexpr bool IsLocallyPossessed() const noexcept { return this->IsOwningNodeValid(); }

    FORCEINLINE constexpr bool IsOwningNodeValid() const noexcept { return this->Node != nullptr; }
    FORCEINLINE WWorldNode* GetOwningNode() noexcept { return this->Node; }
    FORCEINLINE WWorldNode const* GetOwningNode() const noexcept { return this->Node; }
    FORCEINLINE WWorldNode* GetOwningNodeChecked() noexcept{ check(this->Node) return this->Node; }
    FORCEINLINE WWorldNode const* GetOwningNodeChecked() const noexcept { check(this->Node) return this->Node; }
    FORCEINLINE WWorldNode* GetOwningNodeAsserted() noexcept { jassert(this->Node) return this->Node; }
    FORCEINLINE WWorldNode const* GetOwningNodeAsserted() const noexcept { jassert(this->Node) return this->Node; }

    FORCEINLINE bool IsOwnedPawnValid() const noexcept { return this->Pawn.get(); }
    FORCEINLINE APawn* GetOwnedPawn() noexcept { return this->Pawn.get(); }
    FORCEINLINE APawn const* GetOwnedPawn() const noexcept { return this->Pawn.get(); }
    FORCEINLINE APawn* GetOwnedPawnChecked() noexcept { check(this->IsOwnedPawnValid()) return this->Pawn.get(); }
    FORCEINLINE APawn const* GetOwnedPawnChecked() const noexcept { check(this->IsOwnedPawnValid()) return this->Pawn.get(); }
    FORCEINLINE APawn* GetOwnedPawnAsserted() noexcept { jassert(this->IsOwnedPawnValid()) return this->Pawn.get(); }
    FORCEINLINE APawn const* GetOwnedPawnAsserted() const noexcept { jassert(this->IsOwnedPawnValid()) return this->Pawn.get(); }
    void PossessPawn(TJxxUnique<APawn> New, bool bReleaseOld = true);

private:

    WWorldNode* Node{};
    TJxxUnique<APawn> Pawn;
};

} /* ~Namespace Jafg */
