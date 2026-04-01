// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "User/Input/UserInput.h"
#include "Framework/PersonaController.h"
#include "WorldNode.generated.h"

namespace Jafg
{

DECLARE_JAFG_WIDGET()
class ENGINE_API WWorldNode : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WWorldNode)
    {
        this->SetVisibility(ENodeVisibility::Visible);
    }

public:

    virtual void Tick() override;
    virtual void Draw(LNodeRenderInfo const& Info) const override;

    FORCEINLINE LUserInput& GetUserInput() noexcept { return this->UserInput; }
    FORCEINLINE LUserInput const& GetUserInput() const noexcept { return this->UserInput; }

    FORCEINLINE constexpr bool IsOwnedPersonaControllerValid() const noexcept { return this->Controller.get(); }
    FORCEINLINE APersonaController* GetOwnedPersonaController() noexcept { return this->Controller.get(); }
    FORCEINLINE APersonaController const* GetOwnedPersonaController() const noexcept { return this->Controller.get(); }
    FORCEINLINE APersonaController* GetOwnedPersonaControllerChecked() noexcept{ check(this->IsOwnedPersonaControllerValid()) return this->Controller.get(); }
    FORCEINLINE APersonaController const* GetOwnedPersonaControllerChecked() const noexceptcheck { check(this->IsOwnedPersonaControllerValid()) return this->Controller.get(); }
    FORCEINLINE APersonaController* GetOwnedPersonaControllerAsserted() noexcept { jassert(this->IsOwnedPersonaControllerValid()) return this->Controller.get(); }
    FORCEINLINE APersonaController const* GetOwnedPersonaControllerAsserted() const noexcept { jassert(this->IsOwnedPersonaControllerValid()) return this->Controller.get(); }

    ENGINE_API void PossessPersonaController(TJxxUnique<APersonaController> New, bool bReleaseOld = false);

private:

    LUserInput UserInput;
    TJxxUnique<APersonaController> Controller;
};

} /* ~Namespace Jafg */
