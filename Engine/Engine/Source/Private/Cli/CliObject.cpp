// Copyright mzoesch. All rights reserved.

#include "Cli/CliObject.h"
#include "Engine/Engine.h"

Jafg::LCliObject* Jafg::LCliObject::GetRegisteredObjectByUuid() const
{
    if (this->Uuid == LCliObject::NoUuid)
    {
        return nullptr;
    }

    return GEngine->GetCommandLineInterface()->GetObjectAsserted(this->Uuid);
}

void Jafg::LCliObject::ExpandToUuid()
{
    check( this->Uuid == LCliObject::NoUuid )
    const LCliObject* Obj { GEngine->GetCommandLineInterface()->GetObject(this->Identifier) };

    if (Obj == nullptr)
    {
        panicMsgf("Failed to expand object to Uuid from identifier [{}].", this->Identifier)
        return;
    }

    this->Uuid = Obj->Uuid;
    this->Identifier.Empty();

    check( this->Uuid != LCliObject::NoUuid )

    return;
}
