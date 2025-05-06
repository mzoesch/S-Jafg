// Copyright mzoesch. All rights reserved.

#pragma once

#include "CommandLineInterface.h"
#include "User/Preferences/PreferenceValue.h"

namespace Jafg
{

class LPreferenceValue_CliType : public LPreferenceValue
{
public:

    LPreferenceValue_CliType();
};

class LPreferenceValue_CliCommand : public LPreferenceValue
{
public:

    LPreferenceValue_CliCommand();
};

class LPreferenceValue_CliVariable : public LPreferenceValue
{
public:

    LPreferenceValue_CliVariable();
};

} /* ~Namespace Jafg */
