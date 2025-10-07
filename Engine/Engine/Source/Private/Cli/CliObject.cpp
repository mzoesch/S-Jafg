// Copyright mzoesch. All rights reserved.

#include "Cli/CliObject.h"
#include "Engine/Engine.h"

Jafg::LCliObject* Jafg::LCliObject::GetRegisteredObjectByUuid() const
{
    if (this->Uuid == LCliObject::NoUuid)
    {
        return nullptr;
    }

    check( GEngine )
    return GEngine->GetCommandLineInterface()->GetObjectAsserted(*this);
}

void Jafg::LCliObject::ExpandToUuid()
{
    check( this->Uuid == LCliObject::NoUuid )
    check( GEngine )

    const LCliObject* Obj { GEngine->GetCommandLineInterface()->GetObjectAsserted(this->Identifier) };

    this->Uuid = Obj->Uuid;
    algo::orphan(&this->Identifier);

    check( this->Uuid != LCliObject::NoUuid )

    return;
}
