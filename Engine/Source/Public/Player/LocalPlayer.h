// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Object.h"

namespace Jafg
{

class JPlayerInput;

class JLocalPlayer : public JObject
{
public:

    JLocalPlayer() = default;

    void Initialize();
    void Tick(const float DeltaTime);
    void TearDown();

    JPlayerInput* GetPlayerInput(void) const { return this->PlayerInput; }

private:

    /** Only exists if locally controlled. */
    JPlayerInput* PlayerInput;
};

} /* Namespace Jafg. */
