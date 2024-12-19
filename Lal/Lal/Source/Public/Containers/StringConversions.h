// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace EConvErrorHandling
{

enum Type
{
    // Obviously, not every flag can be combined. Use your sense.

    /** Ignore non convertable results. */
    Ignore  = 0x00 << 0,

    /** Emit the non-convertable results to the output. This will produce invalid characters. */
    Emit    = 0x01 << 0,

    /** Warn by emitting a log message. */
    Warn    = 0x01 << 1,

    /** Error by emitting a log message. */
    Error   = 0x01 << 2,

    /** Panic the program. */
    Panic   = 0x01 << 3,

    /** Replace the invalid character / rune with a substring. */
    Replace = 0x01 << 4
};

} /* ~Namespace EConvErrorHandling */

namespace Str
{

LSimpleString ToSimpleString
(
    const LEightString& InEightString,
    const EConvErrorHandling::Type InErrorHandling = EConvErrorHandling::Ignore,
    const char* InReplacement = "?"
);

LEightString ToEightString(const LSimpleString& InSimpleString);

} /* ~Namespace Str */

} /* ~Namespace Jafg */
