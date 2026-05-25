// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

#ifndef SQLITE3_H
    #error "Please include sqlite3 header before including MySqlite3.h."
#endif /* !SQLITE3_H */

namespace Sqlite3Bindings
{

FORCEINLINE int native_open(
#if JAFG_PLATFORM_USES_UTF8
    char const* filename
#elif JAFG_PLATFORM_USES_UTF16
    void const* filename
#else /* JAFG_PLATFORM_USES_UTF16 */
    #error "Unsupported platform encoding."
#endif /* !JAFG_PLATFORM_USES_UTF16 */
    , sqlite3** ppDb
    ) noexcept
{
    return
#if JAFG_PLATFORM_USES_UTF8
    sqlite3_open
#elif JAFG_PLATFORM_USES_UTF16
    sqlite3_open16
#else /* JAFG_PLATFORM_USES_UTF16 */
    #error "Unsupported platform encoding."
#endif /* !JAFG_PLATFORM_USES_UTF16 */
    (
        filename,
        ppDb
    );
}

} /* ~Namespace Sqlite3Bindings */
