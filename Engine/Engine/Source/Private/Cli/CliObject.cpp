// Copyright mzoesch. All rights reserved.

#include "Cli/CliObject.h"
#include "Engine/Engine.h"

void Jafg::LCliObject::ExpandUuid()
{
    check( this->Uuid == LCliObject::NoUuid )
    LCliObject* Obj = GEngine->GetCommandLineInterface()->GetObjectAsserted(this->Identifier);

    this->Uuid = Obj->Uuid;
    this->Identifier.Empty();
    this->Help.Empty();

    check( this->Uuid != LCliObject::NoUuid )

    return;
}
