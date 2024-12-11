// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "ChatScreen.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API WChatScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WChatScreen)

public:

    virtual void Construct() override;
};

} /* ~Namespace Jafg */
