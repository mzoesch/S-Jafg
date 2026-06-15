// Copyright mzoesch. All rights reserved.

#include "User/JgcInputSubsystem.h"
#include "User/LocalEgo.h"
#include "User/Input/CoreInputSubsystem.h"
#include "User/Input/UserInputRegistry.h"
#include "Framework/PersonaController.h"
#include "User/Input/UserInput.h"

void Jgc::JgcInputSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    Collection.InitializeDependency<Jafg::JCoreInputSubsystem>(this);

    // auto& Frontend{this->GetLocalEgo().GetFrontend()};
    // Jafg::LUserInputRegistry& Registry{this->GetMutableLocalEgo().GetUserInputRegistry()};
}
