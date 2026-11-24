// Copyright mzoesch. All rights reserved.

#include "User/ControlsPreferences.h"
#include "User/UserPreferences.h"
#include "User/PreferenceValue.h"
#include "User/LocalEgo.h"
#include "User/Input/UserInput.h"
#include "User/Input/UserInputRegistry.h"
#include "Engine/Engine.h"
#include "Framework/Frontend.h"
#include "Framework/SurfaceGlfw3.h"
#include "Nodes/Box.h"
#include "Nodes/Button.h"
#include "Nodes/DropDown.h"
#include "Nodes/HButton.h"
#include "Nodes/HParent.h"
#include "Nodes/HRegion.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "Nodes/VParent.h"
#include "Widgets/Input_Boolean.h"
#include <charconv>

namespace Jafg::Controls
{

namespace
{

struct LPreferenceValue_KeyBinding;

constexpr LColor WarningTint{ 0xE0A040FF_color };
//# GLFW_KEY_UNKNOWN. Makes the frontend resolve the name of a key by its scancode only.
constexpr i32 ResolveByScancode{ -1 };
constexpr char ChordSeparator{ '|' };

NODISCARD LFrontend const& GetFrontend() noexcept
{
    check(Detail::GMutableEngine)
    return Detail::GMutableEngine->GetLocalEgo().GetFrontend();
}

NODISCARD LUserInputRegistry& GetMutableRegistry() noexcept
{
    check(Detail::GMutableEngine)
    return Detail::GMutableEngine->GetLocalEgo().GetUserInputRegistry();
}

NODISCARD LString ToLowerAscii(LStringView Text)
{
    LString Result{Text};
    for (auto& Char: Result)
    {
        if (Char >= 'A' && Char <= 'Z')
        {
            Char = static_cast<char>(Char - 'A' + 'a');
        }
    }
    return Result;
}

NODISCARD LString ToUpperAscii(LStringView Text)
{
    LString Result{Text};
    for (auto& Char: Result)
    {
        if (Char >= 'a' && Char <= 'z')
        {
            Char = static_cast<char>(Char - 'a' + 'A');
        }
    }
    return Result;
}

NODISCARD bool IsSingleCodePoint(LStringView Text) noexcept
{
    return algo::count_if(Text, [](char Char){ return (static_cast<u8>(Char) & 0xC0) != 0x80; }) == 1;
}

///////////////////////////////////////////////////////////////////////////////
// Key names
///////////////////////////////////////////////////////////////////////////////

struct LKeyName
{
    ELogicalKey Logical;
    LPhysicalKey Physical;
};

//# The physical keys of all logical keys that exist on this platform.
NODISCARD TArray<LKeyName> const& GetKeyNames() noexcept
{
    static const TArray<LKeyName> Names{[]
    {
        TArray<LKeyName> Out;
        for (auto Idx{static_cast<i32>(ELogicalKey::FirstKey)}; Idx <= static_cast<i32>(ELogicalKey::LastKey); ++Idx)
        {
            const auto Logical{static_cast<ELogicalKey>(Idx)};
            if (auto Physical{GetFrontend().GetPhysicalKey(Logical)}; Physical.has_value() && *Physical != LPhysicalKey{})
            {
                Out.push_back({.Logical = Logical, .Physical = *Physical});
            }
        }
        return Out;
    }()};
    return Names;
}

NODISCARD std::optional<ELogicalKey> FindLogicalKey(LPhysicalKey Key) noexcept
{
    if (Key.IsLogical())
    {
        return static_cast<ELogicalKey>(Key.Logical);
    }
    if (auto* Name{algo::find_pointer(GetKeyNames(), Key, &LKeyName::Physical)})
    {
        return Name->Logical;
    }
    return {};
}

//# @return The modifier that the key is, if any.
NODISCARD std::optional<EModBits> GetModifierOfKey(LPhysicalKey Key) noexcept
{
    switch (FindLogicalKey(Key).value_or(ELogicalKey::Unresolved))
    {
    case ELogicalKey::LeftShift:
    case ELogicalKey::RightShift:     return EModBits::Shift;
    case ELogicalKey::LeftControl:
    case ELogicalKey::RightControl:   return EModBits::Control;
    case ELogicalKey::LeftAlt:
    case ELogicalKey::RightAlt:       return EModBits::Alt;
    case ELogicalKey::LeftCommand:
    case ELogicalKey::RightCommand:   return EModBits::Super;
    default:                          return {};
    }
}

struct LModifierName
{
    EModBits Bit;
    LStringView Name;
};
//# In the order they are displayed and serialized.
constexpr LModifierName ModifierNames[]{
    {EModBits::Control, "Ctrl"},
    {EModBits::Shift, "Shift"},
    {EModBits::Alt, "Alt"},
    {EModBits::Super, "Super"},
};

NODISCARD LString ModifiersToString(EModFlags Mods)
{
    LString Out;
    for (auto const& [Bit, Name]: ModifierNames)
    {
        if (Mods & Bit)
        {
            Out += Name;
            Out += '+';
        }
    }
    return Out;
}

NODISCARD LString SerializeKey(LPhysicalKey Key)
{
    if (auto Logical{FindLogicalKey(Key)})
    {
        return LString{LexToString(*Logical)};
    }
    return algo::sprintf("Scancode{}", Key.Scancode);
}

NODISCARD std::optional<LPhysicalKey> ParseKey(LStringView Token) noexcept
{
    for (auto const& Name: GetKeyNames())
    {
        if (LexToString(Name.Logical) == Token)
        {
            return Name.Physical;
        }
    }
    if (Token.starts_with("Scancode"))
    {
        const LStringView Number{Token.substr(LStringView{"Scancode"}.size())};
        i32 Scancode{};
        if (auto [Ptr, Error]{std::from_chars(Number.data(), Number.data() + Number.size(), Scancode)};
            Error == std::errc{} && Ptr == Number.data() + Number.size())
        {
            return LPhysicalKey{.Scancode = Scancode};
        }
    }
    return {};
}

NODISCARD std::optional<LKeyChord> ParseKeyChord(LStringView Text) noexcept
{
    EModFlags Mods;
    for (auto Plus{Text.find('+')}; Plus != LStringView::npos; Plus = Text.find('+'))
    {
        const LStringView Modifier{Text.substr(0, Plus)};
        auto const* Name{algo::find_pointer(ModifierNames, Modifier, &LModifierName::Name)};
        if (!Name)
        {
            return {};
        }
        Mods |= Name->Bit;
        Text = Text.substr(Plus + 1);
    }

    if (auto Key{ParseKey(Text)})
    {
        return LKeyChord{*Key, Mods};
    }
    return {};
}

///////////////////////////////////////////////////////////////////////////////
// Triggers
///////////////////////////////////////////////////////////////////////////////

NODISCARD LString DescribeTriggerKind(EInputActionTriggerFlags Flags)
{
    LString Out;
    auto Add{[&Out](LStringView Kind)
    {
        if (!Out.empty())
        {
            Out += ", ";
        }
        Out += Kind;
    }};
    if (Flags & EInputActionTriggerBits::Triggered) { Add("Press"); }
    if (Flags & EInputActionTriggerBits::Ongoing)   { Add("Hold"); }
    if (Flags & EInputActionTriggerBits::Completed) { Add("Release"); }
    return Out;
}

NODISCARD LString DescribeTriggerBehavior(EInputActionTriggerFlags Flags)
{
    LString Out;
    if (Flags & EInputActionTriggerBits::Triggered) { Out += "Fires when a key is pressed. "; }
    if (Flags & EInputActionTriggerBits::Ongoing)   { Out += "Fires continuously while a key is held. "; }
    if (Flags & EInputActionTriggerBits::Completed) { Out += "Fires when a key is released. "; }
    if (!Out.empty())
    {
        Out.pop_back();
    }
    return Out;
}

NODISCARD LString MakeTriggerKey(LUserInputTag Context, LUserInputTag Action, std::size_t TriggerIndex)
{
    return algo::sprintf("{}/{}/{}", Context.ToString(), Action.ToString(), TriggerIndex);
}

//# Stores the keys as an override of the user, or removes the override if they are the defaults.
void StoreKeyBindingOverride(LUserInputTag Context, LUserInputTag Action, std::size_t TriggerIndex, LInputTrigger const& Trigger)
{
    auto& Bindings{*GetMutableSingleton<JUserPreferences>().KeyBindings};
    const LString Prefix{MakeTriggerKey(Context, Action, TriggerIndex) + '='};

    std::erase_if(Bindings, [&Prefix](LString const& Entry){ return Entry.starts_with(Prefix); });
    if (Trigger.Keys != Trigger.DefaultKeys)
    {
        Bindings.push_back(Prefix + SerializeKeyChords(Trigger.Keys));
        std::ranges::sort(Bindings);
    }

    return;
}

///////////////////////////////////////////////////////////////////////////////
// Preference values
///////////////////////////////////////////////////////////////////////////////

LFactoryNode CreateKeyBindingEditor(LViewport& Viewport, LPreferenceChanges& Changes, LPreferenceValue_KeyBinding& Value);

//# The keys of one trigger of a mapped action. The value is the serialized form of the chords.
struct LPreferenceValue_KeyBinding final : LPreferenceValue
{
    explicit LPreferenceValue_KeyBinding(LString Identifier) noexcept
        : LPreferenceValue{{
            .Identifier = std::move(Identifier),
            .CreateEditNode = [](LViewport& Viewport, LPreferenceChanges& Changes, LPreferenceValue& Preference) -> LFactoryNode
            {
                return CreateKeyBindingEditor(Viewport, Changes, static_cast<LPreferenceValue_KeyBinding&>(Preference));
            },
            }}
    {
    }

    LUserInputTag ContextTag;
    LUserInputTag ActionTag;
    std::size_t TriggerIndex{};
    LString ContextName;
    //# The action and, if the action has multiple triggers, the trigger. E.g., "Moving: Forward".
    LString Label;
    EInputActionTriggerFlags TriggerFlags;

    NODISCARD LInputAction const* FindAction() const noexcept
    {
        if (auto const* Context{GetMutableRegistry().FindContext(this->ContextTag)})
        {
            return Context->FindAction(this->ActionTag);
        }
        return nullptr;
    }

    //# The keys of the trigger are mutable. Everything else of an action is fixed after it was mapped.
    NODISCARD LInputTrigger const* FindTrigger() const noexcept
    {
        if (auto const* Action{this->FindAction()}; Action && algo::valid_index(Action->Triggers, this->TriggerIndex))
        {
            return &Action->Triggers[this->TriggerIndex];
        }
        return nullptr;
    }

    NODISCARD TArray<LKeyChord> GetAppliedKeys() const noexcept
    {
        auto* Trigger{this->FindTrigger()};
        return Trigger ? Trigger->Keys : TArray<LKeyChord>{};
    }

    NODISCARD TArray<LKeyChord> GetDefaultKeys() const noexcept
    {
        auto* Trigger{this->FindTrigger()};
        return Trigger ? Trigger->DefaultKeys : TArray<LKeyChord>{};
    }

    NODISCARD virtual LString GetValue() const override
    {
        return SerializeKeyChords(this->GetAppliedKeys());
    }

    virtual void SaveValue(LString Value) override
    {
        if (auto const* Action{this->FindAction()}; Action && (Action->Flags & EInputActionBits::ReadOnly))
        {
            LOG_WARNING(LogUserInput, "Cannot save key binding [{}]. The action is read-only.", this->Identifier)
            return;
        }
        auto* Trigger{this->FindTrigger()};
        if (!Trigger)
        {
            LOG_WARNING(LogUserInput, "Cannot save key binding [{}]. The trigger does not exist anymore.", this->Identifier)
            return;
        }
        auto Keys{ParseKeyChords(Value)};
        if (!Keys)
        {
            LOG_ERROR(LogUserInput, "Cannot save key binding [{}]. Malformed keys [{}].", this->Identifier, Value)
            return;
        }

        Trigger->Keys = std::move(*Keys);
        StoreKeyBindingOverride(this->ContextTag, this->ActionTag, this->TriggerIndex, *Trigger);
    }
};

///////////////////////////////////////////////////////////////////////////////
// State
///////////////////////////////////////////////////////////////////////////////

//# Which bindings are ambiguous, based on the pending keys of a host.
struct LConflictTable
{
    struct LEntry
    {
        //# The pending keys of the value.
        TArray<LKeyChord> Keys;
        //# Per key: the values that the key is ambiguous with.
        TArray<TArray<LPreferenceValue_KeyBinding const*>> Conflicts;

        NODISCARD bool HasConflicts() const noexcept { return algo::any_of(this->Conflicts, [](auto const& Xs){ return !Xs.empty(); }); }
    };

    std::unordered_map<LPreferenceValue_KeyBinding const*, LEntry> Entries;
    std::size_t AmbiguousKeyCount{};

    NODISCARD LEntry const* Find(LPreferenceValue_KeyBinding const* Value) const noexcept
    {
        if (auto It{this->Entries.find(Value)}; It != this->Entries.end())
        {
            return &It->second;
        }
        return nullptr;
    }
};

struct LHostState
{
    LPreferenceCollectionHost Host;
    std::optional<LConflictTable> Conflicts;
};

struct LControlsState
{
    //# All values of the controls collection. Owned by the collection.
    TArray<LPreferenceValue_KeyBinding*> Values;
    //# The contexts that are listed. Sorted by their display names.
    TArray<std::pair<LUserInputTag, LString>> Contexts;
    std::unordered_map<LPreferenceChanges const*, LHostState> Hosts;

    //# Whether keys are ambiguous regardless of the events (press, hold, release) that their triggers fire on.
    bool bAnyEvent{};
    //# Whether to hide all triggers without ambiguous keys.
    bool bOnlyAmbiguous{};
    //# Whether to hide all triggers of read-only actions.
    bool bOnlyEditable{};
    //# Contexts that are checked together for ambiguous keys. E.g., because they are active at the same time.
    TArray<LUserInputTag> CheckedTogether;
};

NODISCARD LControlsState& GetState() noexcept
{
    static LControlsState State;
    return State;
}

//# The pending keys of a value, if a change exists. Otherwise, the applied keys.
NODISCARD TArray<LKeyChord> GetCurrentKeys(LPreferenceValue_KeyBinding const& Value, LPreferenceChanges const& Changes) noexcept
{
    for (auto const& Change: Changes)
    {
        if (&Change.Preference == static_cast<LPreferenceValue const*>(&Value))
        {
            if (auto Keys{ParseKeyChords(Change.Value)})
            {
                return std::move(*Keys);
            }
            break;
        }
    }
    return Value.GetAppliedKeys();
}

NODISCARD bool DoKeysOverlap(LPhysicalKey Lhs, LPhysicalKey Rhs) noexcept
{
    if (Lhs == Rhs)
    {
        return true;
    }
    auto IsPartOf{[](LPhysicalKey Part, LPhysicalKey Whole)
    {
        return Whole == LPhysicalKey::FromLogical(ELogicalKey::MouseXY)
            && (Part == LPhysicalKey::FromLogical(ELogicalKey::MouseX) || Part == LPhysicalKey::FromLogical(ELogicalKey::MouseY));
    }};
    return IsPartOf(Lhs, Rhs) || IsPartOf(Rhs, Lhs);
}

NODISCARD LConflictTable ComputeConflicts(LPreferenceChanges const& Changes)
{
    auto& State{GetState()};
    LConflictTable Table;

    for (auto const* Value: State.Values)
    {
        auto& Entry{Table.Entries[Value]};
        Entry.Keys = GetCurrentKeys(*Value, Changes);
        Entry.Conflicts.resize(Entry.Keys.size());
    }

    auto IsCheckedTogether{[&State](LUserInputTag Context){ return algo::contains(State.CheckedTogether, Context); }};
    for (auto I{0uz}; I < State.Values.size(); ++I)
    {
        for (auto J{I + 1uz}; J < State.Values.size(); ++J)
        {
            auto const* A{State.Values[I]};
            auto const* B{State.Values[J]};

            if (A->ContextTag != B->ContextTag && !(IsCheckedTogether(A->ContextTag) && IsCheckedTogether(B->ContextTag)))
            {
                continue;
            }
            /* Otherwise, they fire on different events and both get the input. E.g., jump on press and jump held. */
            if (!State.bAnyEvent && !(A->TriggerFlags & B->TriggerFlags))
            {
                continue;
            }

            auto& EntryA{Table.Entries[A]};
            auto& EntryB{Table.Entries[B]};
            for (auto KeyA{0uz}; KeyA < EntryA.Keys.size(); ++KeyA)
            {
                for (auto KeyB{0uz}; KeyB < EntryB.Keys.size(); ++KeyB)
                {
                    /* Different modifiers are not ambiguous. The chord with modifiers takes precedence. */
                    if (EntryA.Keys[KeyA].Mods == EntryB.Keys[KeyB].Mods && DoKeysOverlap(EntryA.Keys[KeyA].Key, EntryB.Keys[KeyB].Key))
                    {
                        EntryA.Conflicts[KeyA].push_back(B);
                        EntryB.Conflicts[KeyB].push_back(A);
                    }
                }
            }
        }
    }

    for (auto const& Entry: Table.Entries | std::views::values)
    {
        Table.AmbiguousKeyCount += static_cast<std::size_t>(algo::count_if(Entry.Conflicts, [](auto const& Xs){ return !Xs.empty(); }));
    }

    return Table;
}

NODISCARD LConflictTable const& GetConflicts(LPreferenceChanges const& Changes)
{
    auto& Host{GetState().Hosts[&Changes]};
    if (!Host.Conflicts)
    {
        Host.Conflicts = ComputeConflicts(Changes);
    }
    return *Host.Conflicts;
}

//# Rebuilds every host, e.g., after the conflict rules changed.
void RebuildAllHosts()
{
    for (auto& Host: GetState().Hosts | std::views::values)
    {
        Host.Conflicts.reset();
        if (Host.Host.RequestRebuild)
        {
            Host.Host.RequestRebuild();
        }
    }
}

//# Records the keys as a pending change of the host and rebuilds it.
void SetPendingKeys(LPreferenceValue_KeyBinding& Value, LPreferenceChanges& Changes, TArray<LKeyChord> const& Keys)
{
    LString Serialized{SerializeKeyChords(Keys)};
    auto It{std::ranges::find_if(Changes, [&Value](LPreferenceValueChangeRequest const& Change)
    {
        return &Change.Preference == static_cast<LPreferenceValue const*>(&Value);
    })};

    if (Serialized == Value.GetValue())
    {
        if (It != Changes.end())
        {
            Changes.erase(It);
        }
    }
    else if (It != Changes.end())
    {
        It->Value = std::move(Serialized);
    }
    else
    {
        Changes.emplace_back(static_cast<LPreferenceValue&>(Value), std::move(Serialized));
    }

    if (auto HostIt{GetState().Hosts.find(&Changes)}; HostIt != GetState().Hosts.end())
    {
        HostIt->second.Conflicts.reset();
        if (HostIt->second.Host.RequestRebuild)
        {
            HostIt->second.Host.RequestRebuild();
        }
    }

    return;
}

NODISCARD LString DescribeConflicts(LPreferenceValue_KeyBinding const& Value, TArray<LPreferenceValue_KeyBinding const*> const& Conflicts)
{
    LString Out{"Ambiguous. Also bound to "};
    for (auto Idx{0uz}; Idx < Conflicts.size(); ++Idx)
    {
        if (Idx > 0uz)
        {
            Out += ", ";
        }
        Out += Conflicts[Idx]->Label;
        if (Conflicts[Idx]->ContextTag != Value.ContextTag)
        {
            Out += " (";
            Out += Conflicts[Idx]->ContextName;
            Out += ')';
        }
    }
    Out += '.';
    return Out;
}

///////////////////////////////////////////////////////////////////////////////
// Nodes
///////////////////////////////////////////////////////////////////////////////

NODISCARD bool IsCursorOver(WNode const& Node, LNodeKeyEventInfo const& Info) noexcept
{
    return Info.CursorLocation.has_value() && Node.AabbTest({.Translation = Info.Translation}, *Info.CursorLocation);
}

NODISCARD TJxxUnique<WNode> CreateWarningIcon(LViewport& Viewport)
{
    return NewNode(Viewport).Class<WBox>()
        .Anchor(EAnchor::VFill)
        .MinDesiredSize({14_spt, 0})
        .Icon(LOptionalTexture2Ref{JafgInfo, "Icons/Warning"})
        .Tint(WarningTint)
        .BorderTint(Colors::Transparent)
        .Unique();
}

//# An item. E.g., a key of a trigger. It has a remove button if #OnRemove is bound.
NODISCARD TJxxUnique<WNode> CreateChip(LViewport& Viewport
    , LString Content
    , LString Tooltip
    , bool bWarning
    , LString RemoveTooltip
    , TFunction2<void()> OnRemove
    )
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    WHRegion* Chip;
    auto Factory{NewNode(Viewport).Class<WHRegion>().SaveTo(&Chip)
        /* Hit-testable, so that the chip has a tooltip. */
        .Visibility(ENodeVisibility::Visible)
        .Anchor(EAnchor::VCenter)
        .Padding({3_spt, 1})
        .Space(3_spt)
        .Tint(*Prefs.InputColor)
        .OutlineThickness(1)
        .OutlineTint(bWarning ? WarningTint : 0x5F5F5FFF_color)
        .Radii(LVec4F{4.0f})
        .Tooltip(std::move(Tooltip))};

    if (bWarning)
    {
        Chip->AddChild(CreateWarningIcon(Viewport));
    }
    Chip->AddChild(NewNode(Viewport).Class<WText>()
        .Anchor(EAnchor::VFill)
        .TextAlign(ETextVAlign::Center)
        .Content(std::move(Content))
        .Unique());
    if (!OnRemove)
    {
        return Factory.Unique();
    }
    Chip->AddChild(NewNode(Viewport).Class<WButton>()
        .Anchor(EAnchor::VFill)
        .MinDesiredSize({12_spt, 0})
        .InAllBrushesChained<&LRegionBrush::BorderTint, &LRegionBrush::Tint>(Colors::Transparent, Colors::Gray)
        .InBrush<EStyleBits::ActiveCombi, &LRegionBrush::Tint>(Colors::White)
        .InAllBrushes<&LRegionBrush::Background>(LRegionBrush::Icon({JafgInfo, "Icons/SmallX"}))
        .Tooltip(std::move(RemoveTooltip))
        .OnKeyEventFocused([OnRemove=std::move(OnRemove)](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event) mutable
        {
            if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton) && IsCursorOver(Self, Info))
            {
                OnRemove();
                return LNodeReply::Handled();
            }
            return LNodeReply::Unhandled();
        })
        .Unique());

    return Factory.Unique();
}

NODISCARD TJxxUnique<WNode> CreateKeyChip(LViewport& Viewport
    , LPreferenceChanges& Changes
    , LPreferenceValue_KeyBinding& Value
    , TArray<LKeyChord> const& Keys
    , std::size_t Index
    , TArray<LPreferenceValue_KeyBinding const*> const& Conflicts
    )
{
    const bool bAmbiguous{!Conflicts.empty()};
    TFunction2<void()> OnRemove;
    if (!Value.bReadOnly)
    {
        OnRemove = [Changes=&Changes, Value=&Value, Keys, Index]
        {
            auto NewKeys{Keys};
            NewKeys.erase(NewKeys.begin() + static_cast<std::ptrdiff_t>(Index));
            SetPendingKeys(*Value, *Changes, NewKeys);
        };
    }
    return CreateChip(Viewport
        , GetKeyChordDisplayName(Keys[Index])
        , bAmbiguous ? DescribeConflicts(Value, Conflicts) : GetKeyChordDisplayName(Keys[Index])
        , bAmbiguous
        , "Remove this key"
        , std::move(OnRemove));
}

//# Opens a list of the contexts that are not checked together yet. Picking one adds it.
NODISCARD TJxxUnique<WNode> CreateAddContextButton(LViewport& Viewport)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto& State{GetState()};

    TArray<std::pair<LUserInputTag, LString>> Available;
    for (auto const& Context: State.Contexts)
    {
        if (!algo::contains(State.CheckedTogether, Context.first))
        {
            Available.push_back(Context);
        }
    }

    const bool bAny{!Available.empty()};
    return NewNode(Viewport).Class<WTextButton>()
        .Anchor(EAnchor::VCenter)
        .Content("+")
        .Palette(*Prefs.SecondaryPaletteSolid)
        .InAllBrushesChained<&LBoxBrush::Radii, &LBoxBrush::Padding>(LVec4F{4.0f}, {4_spt, 0})
        .Enabled(bAny)
        .Tooltip(bAny ? "Add a context" : "All contexts are added")
        .OnKeyEventFocused([Available=std::move(Available)](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
        {
            if (Self.AsStatic<WTextButton>().IsEnabled() && Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton) && IsCursorOver(Self, Info))
            {
                TArray<LDropDownNode> Options;
                Options.reserve(Available.size());
                for (auto const& [Tag, Name]: Available)
                {
                    Options.emplace_back(LDropDownNodeOption{
                        .Selector = {.DisplayName = Name},
                        .OnAction = [Tag](WNode&, LNodeKeyEventInfo const&, LKeyEvent const&) -> algo::reply
                        {
                            if (auto& Checked{GetState().CheckedTogether}; !algo::contains(Checked, Tag))
                            {
                                Checked.push_back(Tag);
                                RebuildAllHosts();
                            }
                            /* Closes the list. */
                            return algo::reply::unhandled();
                        },
                        });
                }

                (void)CreateDropDownMenu(Info.Viewport
                    , Self.GetAnchoredAndTranslatedTopLeftFromMostOuter(Self.GetTranslationFromMostOuter()) + LVec2F{0.0f, Self.GetAnchoredSize_v2().y}
                    , {}
                    , Options
                    );
                return LNodeReply::Handled();
            }
            return LNodeReply::Unhandled();
        })
        .Unique();
}

NODISCARD TJxxUnique<WNode> CreateCaptureButton(LViewport& Viewport, LPreferenceChanges& Changes, LPreferenceValue_KeyBinding& Value, TArray<LKeyChord> const& Keys)
{
    static constexpr LStringView IdleContent{"+"};
    static constexpr LStringView CapturingContent{"Press a key..."};

    struct LCapture
    {
        bool bCapturing{};
        //# A modifier key that was pressed while capturing. Released on its own, it becomes the chord.
        std::optional<LPhysicalKey> PendingModifier;
    };
    auto Capture{std::make_shared<LCapture>()};

    auto& Prefs{GetSingleton<JUserPreferences>()};
    return NewNode(Viewport).Class<WTextButton>()
        .Anchor(EAnchor::VCenter)
        .Content(LString{IdleContent})
        .Palette(*Prefs.SecondaryPaletteSolid)
        .InAllBrushesChained<&LBoxBrush::Radii, &LBoxBrush::Padding>(LVec4F{4.0f}, {4_spt, 0})
        .Tooltip("Add a key: click, then press the key together with its modifiers, or click a mouse button here. Escape cancels.")
        .OnKeyEventFocused([Changes=&Changes, Value=&Value, Keys, Capture](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
        {
            auto& Button{Self.AsStatic<WTextButton>()};

            if (!Capture->bCapturing)
            {
                if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton) && IsCursorOver(Self, Info))
                {
                    Capture->bCapturing = true;
                    Capture->PendingModifier.reset();
                    Button.SetContent(LString{CapturingContent});
                    return LNodeReply::Handled();
                }
                return LNodeReply::Unhandled();
            }

            const LPhysicalKey Key{Event.PhysicalKey};
            if (Key.IsAnyLogicalOf(ELogicalKey::MouseX, ELogicalKey::MouseY))
            {
                return LNodeReply::Unhandled();
            }

            auto Finish{[&](std::optional<LKeyChord> const& Chord)
            {
                Capture->bCapturing = false;
                Capture->PendingModifier.reset();
                Button.SetContent(LString{IdleContent});
                if (Chord && !algo::contains(Keys, *Chord))
                {
                    auto NewKeys{Keys};
                    NewKeys.push_back(*Chord);
                    SetPendingKeys(*Value, *Changes, NewKeys);
                }
                return LNodeReply::Handled();
            }};

            const EModFlags Held{(GetHeldChordModifiers(Info.Surface) | Event.Mods) & ChordModifierMask};
            if (Event.Is<ERawInputStateBits::Press>())
            {
                if (!Held && Info.Frontend.GetPhysicalKey(ELogicalKey::Escape) == Key)
                {
                    return Finish({});
                }
                if (GetModifierOfKey(Key))
                {
                    Capture->PendingModifier = Key;
                    return LNodeReply::Handled();
                }
                return Finish(LKeyChord{Key, Held});
            }
            if (Event.Is<ERawInputStateBits::Release>() && Capture->PendingModifier == Key)
            {
                return Finish(LKeyChord{Key, Held & ~EModFlags{*GetModifierOfKey(Key)}});
            }

            return LNodeReply::Handled();
        })
        .OnFocusLost([Capture](WNode& Self)
        {
            if (Capture->bCapturing)
            {
                Capture->bCapturing = false;
                Capture->PendingModifier.reset();
                if (!Self._IsGarbage())
                {
                    Self.AsStatic<WTextButton>().SetContent(LString{IdleContent});
                }
            }
        })
        .Unique();
}

NODISCARD TJxxUnique<WNode> CreateResetButton(LViewport& Viewport, LPreferenceChanges& Changes, LPreferenceValue_KeyBinding& Value, TArray<LKeyChord> const& Keys)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    auto Defaults{Value.GetDefaultKeys()};
    LString Tooltip{"Reset to default: "};
    if (Defaults.empty())
    {
        Tooltip += "No keys";
    }
    for (auto Idx{0uz}; Idx < Defaults.size(); ++Idx)
    {
        if (Idx > 0uz)
        {
            Tooltip += ", ";
        }
        Tooltip += GetKeyChordDisplayName(Defaults[Idx]);
    }

    return NewNode(Viewport).Class<WButton>()
        .Anchor(EAnchor::VCenter)
        .MinDesiredSize({20_spt, 18})
        .InBrush<EStyleBits::ActiveCombi, &LRegionBrush::BorderTint>(*Prefs.ForegroundColorVariant)
        .InBrush<EStyleBits::InactiveCombi, &LRegionBrush::BorderTint>(Colors::Transparent)
        .InBrush<EStyleBits::ActiveCombi|EStyleBits::Normal, &LRegionBrush::Tint>(Colors::White)
        .InBrush<EStyleBits::Disabled, &LRegionBrush::Tint>(0x4F4F4FFF_color)
        .InAllBrushesChained<&LRegionBrush::Background, &LRegionBrush::Radii>(LRegionBrush::Icon({JafgInfo, "Icons/Reset"}), LVec4F{4.0f})
        .Enabled(Keys != Defaults)
        .Tooltip(std::move(Tooltip))
        .OnKeyEventFocused([Changes=&Changes, Value=&Value, Defaults](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
        {
            if (Self.AsStatic<WButton>().IsEnabled() && Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton) && IsCursorOver(Self, Info))
            {
                SetPendingKeys(*Value, *Changes, Defaults);
                return LNodeReply::Handled();
            }
            return LNodeReply::Unhandled();
        })
        .Unique();
}

LFactoryNode CreateKeyBindingEditor(LViewport& Viewport, LPreferenceChanges& Changes, LPreferenceValue_KeyBinding& Value)
{
    auto const* Entry{GetConflicts(Changes).Find(&Value)};
    const TArray<LKeyChord> Keys{Entry ? Entry->Keys : GetCurrentKeys(Value, Changes)};

    WHParent* Root;
    auto Factory{NewNode(Viewport).Class<WHParent>().SaveTo(&Root)
        .Anchor(EAnchor::VFill)
        .Space(4_spt)};

    for (auto Idx{0uz}; Idx < Keys.size(); ++Idx)
    {
        static const TArray<LPreferenceValue_KeyBinding const*> NoConflicts;
        Root->AddChild(CreateKeyChip(Viewport, Changes, Value, Keys, Idx
            , Entry && algo::valid_index(Entry->Conflicts, Idx) ? Entry->Conflicts[Idx] : NoConflicts
            ));
    }
    if (!Value.bReadOnly)
    {
        Root->AddChild(CreateCaptureButton(Viewport, Changes, Value, Keys));
        Root->AddChild(CreateResetButton(Viewport, Changes, Value, Keys));
    }

    return Factory;
}

NODISCARD TJxxUnique<WNode> CreateCheckRow(LViewport& Viewport, LString Label, bool bValue, LNodeSize1 Indent, TFunction2<void(bool)> OnChanged)
{
    return NewNode(Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(6_spt)
        .Padding(LPadding{ENodeSize::StaticPoints, Indent.Size, 0.0f, 0.0f, 0.0f})
    [
        NewNode(Viewport).Class<WInput_Boolean>()
            .Value(bValue)
            .OnValueChanged([OnChanged=std::make_shared<TFunction2<void(bool)>>(std::move(OnChanged))](WInput_Boolean& Self)
            {
                (*OnChanged)(Self.GetValue());
            })
        +
        NewNode(Viewport).Class<WText>()
            .Anchor(EAnchor::VFill)
            .TextAlign(ETextVAlign::Center)
            .Content(std::move(Label))
    ]
    .Unique();
}

LFactoryNode CreateControlsHeader(LViewport& Viewport, LPreferenceChanges& Changes)
{
    auto& State{GetState()};
    auto const& Table{GetConflicts(Changes)};

    WVParent* Root;
    auto Factory{NewNode(Viewport).Class<WVParent>().SaveTo(&Root)
        .Anchor(EAnchor::HFill)
        .Padding(LPadding{ENodeSize::Points, 0.0f, 3.0f, 0.0f, 6.0f})
        .Space(3_spt)};

    {
        WHParent* Summary;
        Root->AddChild(NewNode(Viewport).Class<WHParent>().SaveTo(&Summary).Anchor(EAnchor::HFill).Unique());
        if (Table.AmbiguousKeyCount > 0uz)
        {
            Summary->AddChild(CreateWarningIcon(Viewport));
        }
        Summary->AddChild(NewNode(Viewport).Class<WText>()
            .Anchor(EAnchor::VFill)
            .TextAlign(ETextVAlign::Center)
            .TextTint(Table.AmbiguousKeyCount > 0uz ? WarningTint : Colors::Gray)
            .Content(Table.AmbiguousKeyCount == 0uz
                ? LString{"No ambiguous keys."}
                : algo::sprintf("{} ambiguous {}.", Table.AmbiguousKeyCount, Table.AmbiguousKeyCount == 1uz ? "key" : "keys"))
            .Unique());
    }

    Root->AddChild(CreateCheckRow(Viewport, "Only show ambiguous keys", State.bOnlyAmbiguous, 0_spt, [](bool bValue)
    {
        GetState().bOnlyAmbiguous = bValue;
        for (auto& Host: GetState().Hosts | std::views::values)
        {
            if (Host.Host.RefreshVisibility)
            {
                Host.Host.RefreshVisibility();
            }
        }
    }));
    Root->AddChild(CreateCheckRow(Viewport, "Only show editable actions", State.bOnlyEditable, 0_spt, [](bool bValue)
    {
        GetState().bOnlyEditable = bValue;
        for (auto& Host: GetState().Hosts | std::views::values)
        {
            if (Host.Host.RefreshVisibility)
            {
                Host.Host.RefreshVisibility();
            }
        }
    }));
    Root->AddChild(CreateCheckRow(Viewport, "Make cross event duplicate keys ambiguous"
        , State.bAnyEvent, 0_spt, [](bool bValue)
    {
        GetState().bAnyEvent = bValue;
        RebuildAllHosts();
    }));

    Root->AddChild(NewNode(Viewport).Class<WText>()
        .Content("Cross search ambiguous keys in:")
        .Unique());
    {
        auto& Prefs{GetSingleton<JUserPreferences>()};

        /* Looks like an input field that holds the contexts. It grows with them. */
        WHButton* Contexts;
        Root->AddChild(NewNode(Viewport).Class<WHButton>().SaveTo(&Contexts)
            /* Not only the field itself, but also the chips and the plus button inside of it are hit-testable. */
            .Visibility(ENodeVisibility::Visible)
            .MinDesiredSize({260_spt, 0})
            .Space(4_spt)
            .Padding(LPadding{ENodeSize::StaticPoints, 4.0f, 3.0f, 4.0f, 3.0f})
            .Palette(*Prefs.InputPaletteSolid)
            .InAllBrushes<&LRegionBrush::Radii>(JUserPreferences::InputRadii)
            .Unique());
        for (auto const& Tag: State.CheckedTogether)
        {
            /* The context may not be registered (anymore). */
            if (auto const* Context{algo::find_pointer(State.Contexts, Tag, algo::pair_first)})
            {
                Contexts->AddChild(CreateChip(Viewport
                    , Context->second
                    , algo::sprintf("Input context [{}].", Tag.ToString())
                    , false
                    , "Remove this context"
                    , [Tag]
                    {
                        algo::erase(&GetState().CheckedTogether, Tag);
                        RebuildAllHosts();
                    }));
            }
        }
        Contexts->AddChild(CreateAddContextButton(Viewport));
    }

    return Factory;
}

NODISCARD bool IsControlsValueShown(LPreferenceValue const& Value, LPreferenceChanges const& Changes)
{
    if (GetState().bOnlyEditable && Value.bReadOnly)
    {
        return false;
    }
    if (!GetState().bOnlyAmbiguous)
    {
        return true;
    }
    auto const* Entry{GetConflicts(Changes).Find(&static_cast<LPreferenceValue_KeyBinding const&>(Value))};
    return Entry && Entry->HasConflicts();
}

void RefreshControls(LPreferenceCollection& Self, LPreferenceCollectionHost Host)
{
    check(Tasks::IsOnMasterThread())
    check(Host.Changes)

    auto& State{GetState()};
    for (auto& Other: State.Hosts | std::views::values)
    {
        Other.Conflicts.reset();
    }
    auto* const Changes{Host.Changes};
    State.Hosts[Changes] = LHostState{.Host = std::move(Host), .Conflicts = {}};

    /* Reuse the values of triggers that still exist. Other hosts may still point to them. */
    std::unordered_map<LString, std::unique_ptr<LPreference>> Pool;
    auto Collect{[&Pool](this auto&& Collect, LPreferenceCollection& Collection) -> void
    {
        for (auto& Child: Collection.Children)
        {
            if (Child->IsCollection())
            {
                Collect(Child->AsCollection());
            }
            else
            {
                LString Identifier{Child->Identifier};
                Pool.emplace(std::move(Identifier), std::move(Child));
            }
        }
    }};
    Collect(Self);
    Self.Children.clear();
    State.Values.clear();
    State.Contexts.clear();

    auto& Registry{GetMutableRegistry()};
    TArray<LInputContext*> Contexts;
    for (auto& Context: Registry.GetMutableRegisteredContexts())
    {
        if (!Context.GetRegisteredActions().empty())
        {
            Contexts.push_back(&Context);
        }
    }
    std::ranges::sort(Contexts, [](LInputContext const* Lhs, LInputContext const* Rhs)
    {
        return ToLowerAscii(Lhs->GetDisplayName()) < ToLowerAscii(Rhs->GetDisplayName());
    });

    for (auto* Context: Contexts)
    {
        const LString ContextIdentifier{Context->GetTag().ToString()};
        const LString ContextName{algo::add_spaces_to_camel_case(Context->GetDisplayName())};
        State.Contexts.emplace_back(Context->GetTag(), ContextName);

        auto& ContextCollection{Self.EmplaceCollection(LPreferenceCollection::CreateInfo{
            .Identifier = ContextIdentifier,
            .DisplayName = ContextName,
            .Tooltip = algo::sprintf("Input context [{}].", ContextIdentifier),
            })};

        for (auto const& Action: Context->GetRegisteredActions())
        {
            const LString ActionName{algo::add_spaces_to_camel_case(Action.DisplayName)};
            const bool bSingleTrigger{Action.Triggers.size() == 1uz && Action.Triggers.front().DisplayName.empty()};

            LPreferenceCollection* Target{&ContextCollection};
            if (!bSingleTrigger)
            {
                Target = &ContextCollection.EmplaceCollection(LPreferenceCollection::CreateInfo{
                    .Identifier = algo::sprintf("{}/{}", ContextIdentifier, Action.Tag.ToString()),
                    .DisplayName = ActionName,
                    .Tooltip = {},
                    });
            }

            for (auto Idx{0uz}; Idx < Action.Triggers.size(); ++Idx)
            {
                auto const& Trigger{Action.Triggers[Idx]};
                LString Identifier{MakeTriggerKey(Context->GetTag(), Action.Tag, Idx)};

                std::unique_ptr<LPreference> Preference;
                if (auto It{Pool.find(Identifier)}; It != Pool.end())
                {
                    Preference = std::move(It->second);
                    Pool.erase(It);
                }
                else
                {
                    Preference = std::make_unique<LPreferenceValue_KeyBinding>(std::move(Identifier));
                }

                auto& Value{static_cast<LPreferenceValue_KeyBinding&>(*Preference)};
                const LString TriggerName{Trigger.DisplayName.empty() ? algo::sprintf("Trigger {}", Idx + 1uz) : Trigger.DisplayName};
                Value.DisplayName = bSingleTrigger ? ActionName : TriggerName;
                Value.Label = bSingleTrigger ? ActionName : algo::sprintf("{}: {}", ActionName, TriggerName);
                Value.Tooltip = DescribeTriggerBehavior(Trigger.TriggerFlags);
                Value.ContextTag = Context->GetTag();
                Value.ActionTag = Action.Tag;
                Value.TriggerIndex = Idx;
                Value.ContextName = ContextName;
                Value.TriggerFlags = Trigger.TriggerFlags;
                Value.Annotation = DescribeTriggerKind(Trigger.TriggerFlags);
                Value.SearchKeywords = Value.Annotation;
                Value.bReadOnly = !!(Action.Flags & EInputActionBits::ReadOnly);
                if (Value.bReadOnly)
                {
                    Value.SearchKeywords += "\nread-only";
                }
                for (auto const& Chord: Trigger.Keys)
                {
                    Value.SearchKeywords += '\n';
                    Value.SearchKeywords += GetKeyChordDisplayName(Chord);
                }

                State.Values.push_back(&Value);
                Target->Children.push_back(std::move(Preference));
            }
        }
    }

    return;
}

} /* ~Namespace <Anonymous> */

} /* ~Namespace Jafg::Controls */

LString Jafg::Controls::GetKeyDisplayName(LPhysicalKey Key) noexcept
{
    const auto Logical{FindLogicalKey(Key)};
    if (Logical == ELogicalKey::MouseXY)
    {
        return "Mouse Movement";
    }

    /* Numpad keys print digits too. Their names keep them apart from the digits of the main block. */
    const bool bNumPad{Logical.has_value() && LexToString(*Logical).starts_with("NumPad")};
    if (!Key.IsLogical() && !bNumPad)
    {
        /* Whitespace, like the space bar, would be invisible. */
        if (auto Localized{GetFrontend().Glfw_GetPhysicalKeyLocalizedRepr(Key, ResolveByScancode)};
            Localized && IsSingleCodePoint(*Localized) && static_cast<u8>(Localized->front()) > static_cast<u8>(' '))
        {
            return ToUpperAscii(*Localized);
        }
    }

    if (Logical)
    {
        return algo::add_spaces_to_camel_case.operator()<LStringView, LString>(LexToString(*Logical));
    }
    return algo::sprintf("Key {}", Key.Scancode);
}

LString Jafg::Controls::GetKeyChordDisplayName(LKeyChord const& Chord) noexcept
{
    return ModifiersToString(Chord.Mods) + GetKeyDisplayName(Chord.Key);
}

LString Jafg::Controls::SerializeKeyChords(TArray<LKeyChord> const& Chords) noexcept
{
    LString Out;
    for (auto Idx{0uz}; Idx < Chords.size(); ++Idx)
    {
        if (Idx > 0uz)
        {
            Out += ChordSeparator;
        }
        Out += ModifiersToString(Chords[Idx].Mods);
        Out += SerializeKey(Chords[Idx].Key);
    }
    return Out;
}

std::optional<TArray<Jafg::LKeyChord>> Jafg::Controls::ParseKeyChords(LStringView Text) noexcept
{
    TArray<LKeyChord> Out;
    if (Text.empty())
    {
        return Out;
    }

    while (true)
    {
        const auto Separator{Text.find(ChordSeparator)};
        auto Chord{ParseKeyChord(Text.substr(0, Separator))};
        if (!Chord)
        {
            return {};
        }
        Out.push_back(*Chord);

        if (Separator == LStringView::npos)
        {
            break;
        }
        Text = Text.substr(Separator + 1);
    }

    return Out;
}

void Jafg::Controls::InstallControlsCollection(LPreferenceCollection& Collection) noexcept
{
    Collection.Refresh = &RefreshControls;
    Collection.ReleaseHost = [](LPreferenceChanges const& Changes){ GetState().Hosts.erase(&Changes); };
    Collection.CreateHeaderNode = &CreateControlsHeader;
    Collection.IsValueShown = &IsControlsValueShown;
}
