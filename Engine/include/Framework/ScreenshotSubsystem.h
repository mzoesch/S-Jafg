// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendSubsystem.h"
#include "Framework/SurfaceForward.h"
#include "Rhi/RendererCore.h"
#include "User/Input/InputTypes.h"
#include <ctime>
#include "ScreenshotSubsystem.generated.h"

namespace Jafg
{

namespace Screenshots
{

//#
//# The file name (without the extension) of a screenshot taken at the given local time. In the pattern, %Y (year),
//# %m (month), %d (day), %H (hour), %M (minute) and %S (second) are replaced with the time, with leading zeros, and %%
//# with a percent sign. Everything else stays, except characters that file names cannot have: They become underscores.
//# Leading and trailing whitespace and trailing dots are removed.
//#
//# @return Empty if nothing of the pattern is left.
//#
NODISCARD ENGINE_API LString FormatName(LStringView Pattern, std::tm const& LocalTime);

//#
//# Writes the bytes to a new file Name.Extension in the directory, or, if that exists, Name_1.Extension,
//# Name_2.Extension, and so on. The directory is created if it does not exist. Creating the file claims its name, also
//# against others that write at the same time.
//#
//# @param Extension With the dot, e.g. ".png".
//# @return The path of the written file. Empty if it could not be written.
//#
NODISCARD ENGINE_API std::optional<LPath> WriteUniqueFile(LPath const& Directory, LStringView Name, LStringView Extension, std::span<u8 const> Bytes);

//#
//# The source, scaled down to the given height with its aspect ratio if it is taller. Never scaled up.
//# @param MaxHeight Zero: No limit.
//#
NODISCARD ENGINE_API rhi::extent2 GetLimitedExtent(rhi::extent2 Source, u32 MaxHeight) noexcept;

} /* ~Namespace Screenshots */

//#
//# Takes screenshots of a surface, of everything it shows (including the user interface), when the user presses the
//# screenshot key: F12 by default, the [Application] input context in the controls. The key works on every surface,
//# whatever user input contexts are active and whatever consumed the key.
//#
//# Screenshots are saved as PNG to #finder::screenshots_dir. Their name and largest resolution are user preferences
//# (@see #JUserPreferences::ScreenshotNamePattern and #JUserPreferences::ScreenshotMaxHeight). They are encoded and
//# written on the worker thread.
//#
DECLARE_JAFG_CLASS()
class JScreenshotSubsystem final : public JFrontendSubsystem
{
    GENERATED_CLASS_BODY(ENGINE_API)

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JScreenshotSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void Tick(f32 Dt) override;

public:

    //#
    //# Takes a screenshot of the next frame that the surface presents, with the preferences of the user.
    //# @return False if the surface cannot be captured.
    //#
    ENGINE_API bool TakeScreenshot(LSurface& Surface);

private:

    NODISCARD bool IsScreenshotKeyPressed(LSurface const& Surface) const noexcept;

    LUserInputTag ContextTag;
    LUserInputTag ActionTag;
};

} /* ~Namespace Jafg */
