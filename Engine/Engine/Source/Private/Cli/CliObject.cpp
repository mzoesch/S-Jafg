// Copyright mzoesch. All rights reserved.

#include "Cli/CliObject.h"
#include "Engine/Engine.h"

void Jafg::LCliObject::ExpandToUuid()
{
    check( this->Uuid == LCliObject::NoUuid )
    LCliObject* Obj = GEngine->GetCommandLineInterface()->GetObjectAsserted(this->Identifier);

    this->Uuid = Obj->Uuid;
    this->Identifier.Empty();

    check( this->Uuid != LCliObject::NoUuid )

    return;
}
