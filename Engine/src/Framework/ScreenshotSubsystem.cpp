// Copyright mzoesch. All rights reserved.

#include "Framework/ScreenshotSubsystem.h"
#include "Framework/Frontend.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "User/Input/UserInput.h"
#include "User/Input/UserInputRegistry.h"
#include "Core/TaskUtility.h"
#include "Core/Finder2.h"
#include <cerrno>
#include <cstdio>
#include <cstring>

#include "Definitions/PushNoWarnings.h"
    /* Implemented in Texture2.cpp. */
    #include <stb_image_write.h>
    #define STB_IMAGE_RESIZE_IMPLEMENTATION
    #include <stb_image_resize2.h>
#include "Definitions/PopDiagnostics.h"

namespace
{

NODISCARD constexpr bool IsAllowedInFileName(char const C) noexcept
{
    if (static_cast<u8>(C) < 0x20 || static_cast<u8>(C) == 0x7F)
    {
        return false;
    }
    /* Everything that one of the platforms does not allow. */
    switch (C)
    {
    case '<': case '>': case ':': case '"': case '/': case '\\': case '|': case '?': case '*':
    {
        return false;
    }
    default:
    {
        return true;
    }
    }
}

NODISCARD std::tm GetLocalTime(std::time_t const Time) noexcept
{
    std::tm Result{};
#if JAFG_PLATFORM_WINDOWS
    localtime_s(&Result, &Time);
#else /* JAFG_PLATFORM_WINDOWS */
    localtime_r(&Time, &Result);
#endif /* !JAFG_PLATFORM_WINDOWS */
    return Result;
}

//# Opens the file for writing only if it does not exist yet.
NODISCARD std::FILE* OpenNewFile(LPath const& Path) noexcept
{
#if JAFG_PLATFORM_WINDOWS
    return _wfopen(Path.c_str(), L"wbx");
#else /* JAFG_PLATFORM_WINDOWS */
    return std::fopen(Path.c_str(), "wbx");
#endif /* !JAFG_PLATFORM_WINDOWS */
}

struct LScreenshotRequest final
{
    LPath Directory;
    LString Name;
    u32 MaxHeight;
};

//# On the worker thread.
void SaveScreenshot(LScreenshotRequest const& Request, Jafg::LSurfaceCapture Capture)
{
    STAT_FUNCTION()

    using namespace Jafg;

    Capture.ToRgba8();
    const rhi::extent2 Extent{Screenshots::GetLimitedExtent(Capture.Extent, Request.MaxHeight)};

    std::span<u8 const> Pixels{Capture.Bytes};
    TArray<u8> Scaled;
    if (Extent != Capture.Extent)
    {
        Scaled.resize(static_cast<std::size_t>(Extent.width) * Extent.height * 4uz);
        /* The values are as the surface showed them, so they are sRGB encoded. The alpha is opaque everywhere. */
        if (!stbir_resize_uint8_srgb(
              Capture.Bytes.data(), static_cast<int>(Capture.Extent.width), static_cast<int>(Capture.Extent.height), 0
            , Scaled.data(), static_cast<int>(Extent.width), static_cast<int>(Extent.height), 0
            , STBIR_RGBA_PM
            ))
        {
            LOG_ERROR(LogFrontend, "Failed to scale screenshot [{}] from [{}x{}] to [{}x{}]."
                , Request.Name, Capture.Extent.width, Capture.Extent.height, Extent.width, Extent.height
                )
            return;
        }
        Pixels = Scaled;
    }

    TArray<u8> Png;
    if (!stbi_write_png_to_func([](void* Context, void* Data, int Size)
        {
            auto& Out{*static_cast<TArray<u8>*>(Context)};
            auto const* const Bytes{static_cast<u8 const*>(Data)};
            Out.insert(Out.end(), Bytes, Bytes + Size);
        }, &Png, static_cast<int>(Extent.width), static_cast<int>(Extent.height), 4, Pixels.data(), static_cast<int>(Extent.width * 4u)))
    {
        LOG_ERROR(LogFrontend, "Failed to encode screenshot [{}] ([{}x{}]).", Request.Name, Extent.width, Extent.height)
        return;
    }

    if (auto const Path{Screenshots::WriteUniqueFile(Request.Directory, Request.Name, ".png", Png)})
    {
        LOG_INFO(LogFrontend, "Saved screenshot [{}] ([{}x{}]).", Path->string(), Extent.width, Extent.height)
    }
    else
    {
        LOG_ERROR(LogFrontend, "Failed to save screenshot [{}] to [{}].", Request.Name, Request.Directory.string())
    }

    return;
}

} /* ~Namespace <Anonymous> */

LString Jafg::Screenshots::FormatName(LStringView Pattern, std::tm const& LocalTime)
{
    LString Result;
    Result.reserve(Pattern.size() + 8uz);

    for (std::size_t Idx{0}; Idx < Pattern.size(); ++Idx)
    {
        if (const char C{Pattern[Idx]}; C != '%' || Idx + 1uz == Pattern.size())
        {
            Result.push_back(IsAllowedInFileName(C) ? C : '_');
            continue;
        }

        std::optional<i32> Value;
        i32 Width{2};
        switch (Pattern[Idx + 1uz])
        {
        case 'Y': { Value = LocalTime.tm_year + 1900; Width = 4; break; }
        case 'm': { Value = LocalTime.tm_mon + 1; break; }
        case 'd': { Value = LocalTime.tm_mday; break; }
        case 'H': { Value = LocalTime.tm_hour; break; }
        case 'M': { Value = LocalTime.tm_min; break; }
        case 'S': { Value = LocalTime.tm_sec; break; }
        case '%':
        {
            Result.push_back('%');
            ++Idx;
            continue;
        }
        default:
        {
            /* Not a placeholder. Kept as it is. */
            Result.push_back('%');
            continue;
        }
        }

        Result += algo::sprintf("{:0{}}", *Value, Width);
        ++Idx;
    }

    /* Some platforms do not allow these at the end. And names should not start with whitespace either. */
    while (!Result.empty() && (Result.back() == ' ' || Result.back() == '.'))
    {
        Result.pop_back();
    }
    if (const auto First{Result.find_first_not_of(' ')}; First != LString::npos)
    {
        Result.erase(0uz, First);
    }
    else
    {
        Result.clear();
    }

    return Result;
}

std::optional<LPath> Jafg::Screenshots::WriteUniqueFile(LPath const& Directory, LStringView Name, LStringView Extension, std::span<u8 const> Bytes)
{
    check(!Name.empty())

    std::error_code Error;
    finder::create_directories(Directory, Error);
    if (Error)
    {
        LOG_ERROR(LogFrontend, "Failed to create directory [{}]: [{}].", Directory.string(), Error.message())
        return {};
    }

    /* Enough for every sane amount of files with the same name. Beyond, something else went wrong. */
    constexpr u32 MaxSuffix{10'000u};
    for (u32 Suffix{0}; Suffix < MaxSuffix; ++Suffix)
    {
        const LPath Path{Directory / (Suffix == 0u
            ? algo::sprintf("{}{}", Name, Extension)
            : algo::sprintf("{}_{}{}", Name, Suffix, Extension)
            )};

        std::FILE* const File{OpenNewFile(Path)};
        if (!File)
        {
            if (errno == EEXIST)
            {
                continue;
            }
            LOG_ERROR(LogFrontend, "Failed to create file [{}]: [{}].", Path.string(), std::strerror(errno))
            return {};
        }

        const bool bWritten{std::fwrite(Bytes.data(), 1uz, Bytes.size(), File) == Bytes.size()};
        if (std::fclose(File) != 0 || !bWritten)
        {
            LOG_ERROR(LogFrontend, "Failed to write file [{}].", Path.string())
            finder::remove(Path, Error);
            return {};
        }

        return Path;
    }

    LOG_ERROR(LogFrontend, "Failed to find a free name for [{}] in [{}].", Name, Directory.string())
    return {};
}

rhi::extent2 Jafg::Screenshots::GetLimitedExtent(rhi::extent2 Source, u32 MaxHeight) noexcept
{
    if (MaxHeight == 0u || Source.height <= MaxHeight)
    {
        return Source;
    }

    const f64 Width{static_cast<f64>(Source.width) * MaxHeight / Source.height};
    return rhi::extent2{maths::max(1u, static_cast<u32>(std::lround(Width))), MaxHeight};
}

void Jafg::JScreenshotSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->SetShouldTick(true);

    LUserInputRegistry& Registry{this->GetMutableLocalEgo().GetUserInputRegistry()};
    auto const& Frontend{this->GetLocalEgo().GetFrontend()};

    /*
     * Never activated: #Tick reads the keys of the action itself, so that they work everywhere. The context exists, so
     * that the user can change the keys in the controls.
     */
    if (LInputContext* Context{Registry.RegisterContext("Application")})
    {
        this->ContextTag = Context->GetTag();
        this->ActionTag = Context->MapAction({
            .Tag = LUserInputTag::ToTag("TakeScreenshot"),
            .Category = EInputActionCategory::Boolean,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Frontend.GetPhysicalKey(ELogicalKey::F12).value()},
                    }),
            .Callback = [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult { return {}; },
            }).Tag;
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [Application] user input context. Screenshots cannot be taken with a key.")
    }

    return;
}

void Jafg::JScreenshotSubsystem::Tick(f32 Dt)
{
    Super::Tick(Dt);

    for (auto const& Surface: this->GetFrontend().GetSurfaces())
    {
        if (this->IsScreenshotKeyPressed(*Surface))
        {
            this->TakeScreenshot(*Surface);
        }
    }

    return;
}

bool Jafg::JScreenshotSubsystem::TakeScreenshot(LSurface& Surface)
{
    check(Tasks::IsOnMasterThread())

    auto const& Prefs{GetSingleton<JUserPreferences>()};

    /* The time of the request, not of the write. */
    const std::tm Time{::GetLocalTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))};
    LString Name{Screenshots::FormatName(*Prefs.ScreenshotNamePattern, Time)};
    if (Name.empty())
    {
        LOG_WARNING(LogFrontend, "The screenshot name pattern [{}] leaves no name. Using the default pattern [{}]."
            , *Prefs.ScreenshotNamePattern, Prefs.ScreenshotNamePattern.GetDefault()
            )
        Name = Screenshots::FormatName(Prefs.ScreenshotNamePattern.GetDefault(), Time);
    }
    check(!Name.empty())

#if JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    LScreenshotRequest Request{
        .Directory = finder::screenshots_dir(),
        .Name = Name,
        .MaxHeight = *Prefs.ScreenshotMaxHeight,
        };
    if (!Surface.RequestCapture([Request=std::move(Request)](LSurfaceCapture Capture) mutable
        {
            Tasks::Make(ENamedThreads::WorkerThread, ETaskTime::Whenever
                , [Request=std::move(Request), Capture=std::move(Capture)]() mutable
                {
                    ::SaveScreenshot(Request, std::move(Capture));
                });
        }))
    {
        LOG_WARNING(LogFrontend, "[{}]: Cannot take a screenshot, as the surface cannot capture its frames.", Surface.GetHumanReadableName())
        return false;
    }

    LOG_VERBOSE(LogFrontend, "[{}]: Taking screenshot [{}].", Surface.GetHumanReadableName(), Name)
    return true;
#else /* JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
    LOG_WARNING(LogFrontend, "[{}]: Cannot take screenshot [{}], as this platform does not support it.", Surface.GetHumanReadableName(), Name)
    return false;
#endif /* !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
}

bool Jafg::JScreenshotSubsystem::IsScreenshotKeyPressed(LSurface const& Surface) const noexcept
{
    if (!this->ContextTag.IsSet())
    {
        return false;
    }

    LInputContext const* const Context{this->GetLocalEgo().GetUserInputRegistry().FindContext(this->ContextTag)};
    if (!Context)
    {
        return false;
    }
    LInputAction const* const Action{algo::find_pointer(Context->GetRegisteredActions(), this->ActionTag, &LInputAction::Tag)};
    if (!Action)
    {
        return false;
    }

    /* As the user input contexts match chords (@see #LUserInput). */
    const EModFlags HeldMods{GetHeldChordModifiers(Surface)};
    for (LRawInput const& Input: Surface.GetRawInputs())
    {
        if ((Input.State & ERawInputStateBits::Press) == ERawInputStateBits::Identity)
        {
            continue;
        }
        for (auto const& Trigger: Action->Triggers)
        {
            for (auto const& Chord: Trigger.Keys)
            {
                if (Chord.Key == Input.PhysicalKey && Chord.AreModifiersHeld(HeldMods))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
