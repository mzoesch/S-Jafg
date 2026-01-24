// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliType.h"

namespace Jafg
{

class LCommandLineInterface;

template<> FORCEINLINE LCliType LCliType::Type<i64>     () { return LCliType::Type("Integer");  }
template<> FORCEINLINE LCliType LCliType::Type<u64>     () { return LCliType::Type("UInteger"); }
template<> FORCEINLINE LCliType LCliType::Type<u8>      () { return LCliType::Type("Byte");     }
template<> FORCEINLINE LCliType LCliType::Type<f32>     () { return LCliType::Type("Float");    }
template<> FORCEINLINE LCliType LCliType::Type<LString> () { return LCliType::Type("String");   }
template<> FORCEINLINE LCliType LCliType::Type<bool>    () { return LCliType::Type("Bool");     }

namespace Private
{

ENGINE_API void AddPrimitivesToCli(LCommandLineInterface* Cli);

} /* ~Namespace Private */

} /* ~Namespace Jafg */
