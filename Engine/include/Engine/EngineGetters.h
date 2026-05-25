// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

class LEngine;
class LLocalEgo;
class LCommandLineInterface;

struct LEngineGetters
{
    NODISCARD ENGINE_API LEngine const& GetEngine() const noexcept;
    NODISCARD ENGINE_API LEngine& GetMutableEngine() noexcept;

    NODISCARD ENGINE_API LLocalEgo const& GetLocalEgo() const noexcept;
    NODISCARD ENGINE_API LLocalEgo& GetMutableLocalEgo() noexcept;

    NODISCARD ENGINE_API LCommandLineInterface const& GetCommandLineInterface() const noexcept;
    NODISCARD ENGINE_API LCommandLineInterface& GetMutableCommandLineInterface() noexcept;
};

} /* ~Namespace Jafg */

