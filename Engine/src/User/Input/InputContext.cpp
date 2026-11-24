// Copyright mzoesch. All rights reserved.

#include "User/Input/InputContext.h"
#include "User/Input/UserInput.h"
#include "User/ControlsPreferences.h"
#include "User/UserPreferences.h"
#include <charconv>

void Jafg::LInputContext::OnActionMapped(LInputAction& Action) noexcept
{
    check(Tasks::IsOnMasterThread())
    check(algo::contains(this->Actions, Action.Tag, &LInputAction::Tag))

    for (auto const& Trigger: Action.Triggers)
    {
        check(Trigger.Keys.empty())
        Trigger.Keys = Trigger.DefaultKeys;
    }

    const LString Prefix{algo::sprintf("{}/{}/", this->Tag.ToString(), Action.Tag.ToString())};
    for (LString const& UserBinding: *GetSingleton<JUserPreferences>().KeyBindings)
    {
        if (!UserBinding.starts_with(Prefix))
        {
            continue;
        }
        if (Action.Flags & EInputActionBits::ReadOnly)
        {
            LOG_WARNING(LogUserInput, "[{}]: The action is read-only. Ignoring.", UserBinding)
            continue;
        }

        const LStringView Rest{LStringView{UserBinding}.substr(Prefix.size())};
        const auto Equals{Rest.find('=')};
        if (Equals == LStringView::npos)
        {
            LOG_WARNING(LogUserInput, "[{}]: Malformed. Ignoring.", UserBinding)
            continue;
        }

        std::size_t Index{};
        if (auto [Ptr, Error]{std::from_chars(Rest.data(), Rest.data() + Equals, Index)};
            Error != std::errc{} || Ptr != Rest.data() + Equals)
        {
            LOG_WARNING(LogUserInput, "[{}]: Malformed. Ignoring.", UserBinding)
            continue;
        }
        if (!algo::valid_index(Action.Triggers, Index))
        {
            LOG_WARNING(LogUserInput, "[{}]: The action has no such trigger. Ignoring.", UserBinding)
            continue;
        }

        auto Keys{Controls::ParseKeyChords(Rest.substr(Equals + 1))};
        if (!Keys)
        {
            LOG_WARNING(LogUserInput, "[{}]: Malformed keys [{}]. Ignoring.", UserBinding, Rest.substr(Equals + 1))
            continue;
        }
        LOG_VERBOSE(LogUserInput, "[{}]: Applying user key binding [{}].", UserBinding, Rest.substr(Equals + 1))
        Action.Triggers[Index].Keys = std::move(*Keys);
    }

    return;
}
