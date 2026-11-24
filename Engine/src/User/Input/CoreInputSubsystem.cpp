// Copyright mzoesch. All rights reserved.

#include "User/Input/CoreInputSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/Frontend.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "User/Input/UserInputRegistry.h"
#include "Engine/Engine.h"
#include "Framework/PersonaController.h"
#include "User/Input/UserInput.h"
#include "Framework/Editor.h"

namespace
{

//# Calls #Functor with the editor camera component of the actor that the acting controller possesses.
Jafg::LOnUserInputActionResult ForEditorCamera(Jafg::LInputCallback const& Data, auto&& Functor)
{
    if (auto* Possessed{Data.Controller.GetPossessedActor()})
    {
        if (auto* Comp{Possessed->GetComponent<Jafg::AEditorCameraComponent>()})
        {
            Functor(*Comp);
        }
        else
        {
            LOG_WARNING(LogUserInput,
                "Editor camera action was triggered but the possessed actor [{}] has no [{}].",
                Possessed->GetNameAsString(), algo::type_name<Jafg::AEditorCameraComponent>()
                )
        }
    }
    else
    {
        LOG_WARNING(LogUserInput,
            "Editor camera action was triggered but controller [{}] does not possess anything to move.",
            Data.Controller.GetNameAsString()
            )
    }
    return {};
}

//# Makes the mesh material preference of the editor the material instance with the given name.
void SetMeshMaterialPreference(LStringView MaterialName)
{
    LOG_VERBOSE(LogUserInput, "Setting mesh material preference to [{}].", MaterialName)
    check(Jafg::Detail::GMutableEngine)
    Jafg::GetMutableSingleton<Jafg::JUserPreferences>().EditorMeshMaterialPreference =
        Jafg::Detail::GMutableEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<Jafg::JMaterialSubsystem>()
        ->GetInstanceFromName(JafgInfo, MaterialName);
}

} /* ~Namespace <Anonymous> */

void Jafg::JCoreInputSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    LUserInputRegistry& Registry{this->GetMutableLocalEgo().GetUserInputRegistry()};

    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    auto Key{[&Frontend](ELogicalKey Logical) { return Frontend.GetPhysicalKey(Logical).value(); }};

    if (LInputContext* Context{Registry.RegisterContext("RhiDebug", "Rhi Debug")})
    {
        Context->MapAction({
            .Tag = LUserInputTag::ToTag("ToggleRhiPolygonMode"),
            .Category = EInputActionCategory::Boolean,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Key(ELogicalKey::F1)},
                    }),
            .Callback = [](LInputCallback const& Callback, LInputActionValue&) -> LOnUserInputActionResult
            {
                auto& Prefs{GetMutableSingleton<JUserPreferences>()};
                if (Prefs.PolygonMode == EPolygonMode::Fill)
                {
                    LOG_VERBOSE(LogUserInput, "Switching polygon mode to wireframe.")
                    Prefs.PolygonMode = EPolygonMode::Wireframe;
                }
                else
                {
                    LOG_VERBOSE(LogUserInput, "Switching polygon mode to fill.")
                    Prefs.PolygonMode = EPolygonMode::Fill;
                }
                Prefs.OnPolygonModeChanged.Broadcast(&Callback.Controller, *Prefs.PolygonMode);
                return {};
            },});

        Context->MapAction({
            .Tag = LUserInputTag::ToTag("ClearMeshMaterialPreference"),
            .Category = EInputActionCategory::Boolean,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Key(ELogicalKey::F9)},
                    }),
            .Callback = [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
            {
                LOG_VERBOSE(LogUserInput, "Clearing mesh material preference.")
                GetMutableSingleton<JUserPreferences>().EditorMeshMaterialPreference.reset();
                return {};
            },});

        Context->MapAction({
            .Tag = LUserInputTag::ToTag("SetNormalMeshMaterialPreference"),
            .Category = EInputActionCategory::Boolean,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Key(ELogicalKey::F5)},
                    }),
            .Callback = [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
            {
                ::SetMeshMaterialPreference("Materials/Mesh.Normal");
                return {};
            },});

        Context->MapAction({
            .Tag = LUserInputTag::ToTag("SetUVMeshMaterialPreference"),
            .Category = EInputActionCategory::Boolean,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Key(ELogicalKey::F6)},
                    }),
            .Callback = [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
            {
                ::SetMeshMaterialPreference("Materials/Mesh.UV");
                return {};
            },});

        Context->MapAction({
            .Tag = LUserInputTag::ToTag("SetTangentMaterialPreference"),
            .Category = EInputActionCategory::Boolean,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Key(ELogicalKey::F7)},
                    }),
            .Callback = [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
            {
                ::SetMeshMaterialPreference("Materials/Mesh.Tangent");
                return {};
            },});

        Context->MapAction({
            .Tag = LUserInputTag::ToTag("SetHandednessMaterialPreference"),
            .Category = EInputActionCategory::Boolean,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {Key(ELogicalKey::F8)},
                    }),
            .Callback = [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult
            {
                ::SetMeshMaterialPreference("Materials/Mesh.Handedness");
                return {};
            },});
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [RhiDebug] user input context.")
    }

    if (LInputContext* Context{Registry.RegisterContext("EditorCameraCapturer")})
    {
        Context->MapAction({
            .Tag = LUserInputTag::ToTag("MouseCapture"),
            .Category = EInputActionCategory::Boolean,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)},
                    }),
            .Callback = [](LInputCallback const& Data, LInputActionValue&) -> LOnUserInputActionResult
            {
                Data.UserInput.ActivateContext("EditorCamera");
                Data.UserInput.SetConsumeMouse(true);
                return {.bDirty=true};
            },});
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [EditorCameraCapturer] user input context.")
    }

    if (LInputContext* Context{Registry.RegisterContext("EditorCamera")})
    {
        Context->MapAction({
            .Tag = LUserInputTag::ToTag("MouseUncapture"),
            .Category = EInputActionCategory::Boolean,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Completed,
                    .DefaultKeys = {LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)},
                    }),
            .Callback = [](LInputCallback const& Data, LInputActionValue&) -> LOnUserInputActionResult
            {
                Data.UserInput.DeactivateContext("EditorCamera");
                Data.UserInput.SetConsumeMouse(false);
                return {.bDirty=true};
            },});

        Context->MapAction({
            .Tag = LUserInputTag::ToTag("Moving"),
            .Category = EInputActionCategory::Axis3D,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .DisplayName = "Forward",
                    .TriggerFlags = EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Key(ELogicalKey::W)},
                    })
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .DisplayName = "Left",
                    .TriggerFlags = EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Key(ELogicalKey::A)},
                    .Modifiers = LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeInputModifier<LInputActionMappedKeySwizzleXYModifier>())
                        .reflexive_emplace_back(MakeInputModifier<LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .DisplayName = "Backwards",
                    .TriggerFlags = EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Key(ELogicalKey::S)},
                    .Modifiers = LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeInputModifier<LInputActionMappedKeyNegateModifier>())
                    })
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .DisplayName = "Right",
                    .TriggerFlags = EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Key(ELogicalKey::D)},
                    .Modifiers = LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeInputModifier<LInputActionMappedKeySwizzleXYModifier>())
                    })
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .DisplayName = "Up",
                    .TriggerFlags = EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Key(ELogicalKey::E)},
                    .Modifiers = LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeInputModifier<LInputActionMappedKeySwizzleXZModifier>())
                    })
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .DisplayName = "Down",
                    .TriggerFlags = EInputActionTriggerBits::Ongoing,
                    .DefaultKeys = {Key(ELogicalKey::Q)},
                    .Modifiers = LInputActionMappedTriggerModifiers{}
                        .reflexive_emplace_back(MakeInputModifier<LInputActionMappedKeySwizzleXZModifier>())
                        .reflexive_emplace_back(MakeInputModifier<LInputActionMappedKeyNegateModifier>())
                    }),
            .Callback = [](LInputCallback const& Data, LInputActionValue& Value) -> LOnUserInputActionResult
            {
                return ::ForEditorCamera(Data, [&Value](AEditorCameraComponent& Comp) { Comp.OnMove(Value); });
            },});

        Context->MapAction({
            .Tag = LUserInputTag::ToTag("VelocityIncrease"),
            .Category = EInputActionCategory::Axis1D,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {LPhysicalKey::FromLogical(ELogicalKey::MouseWheelUp)},
                    }),
            .Callback = [](LInputCallback const& Data, LInputActionValue& Value) -> LOnUserInputActionResult
            {
                return ::ForEditorCamera(Data, [&Value](AEditorCameraComponent& Comp) { Comp.OnVelocityMultiplierChange(Value); });
            },});

        Context->MapAction({
            .Tag = LUserInputTag::ToTag("VelocityDecrease"),
            .Category = EInputActionCategory::Axis1D,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {LPhysicalKey::FromLogical(ELogicalKey::MouseWheelDown)},
                    }),
            .Callback = [](LInputCallback const& Data, LInputActionValue& Value) -> LOnUserInputActionResult
            {
                return ::ForEditorCamera(Data, [&Value](AEditorCameraComponent& Comp) { Comp.OnVelocityMultiplierChange(Value); });
            },});

        Context->MapAction({
            .Tag = LUserInputTag::ToTag("Rotating"),
            .Category = EInputActionCategory::Axis2D,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = {LPhysicalKey::FromLogical(ELogicalKey::MouseXY)},
                    }),
            .Callback = [](LInputCallback const& Data, LInputActionValue& Value) -> LOnUserInputActionResult
            {
                return ::ForEditorCamera(Data, [&Value](AEditorCameraComponent& Comp) { Comp.OnRotate(Value); });
            },});
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [EditorCamera] user input context.")
    }

    /*
     * Shortcuts that the user interface handles itself, without user input contexts. The following contexts are never
     * activated. They exist, so that the preferences list these keys and can check them for duplicates. Their actions
     * are read-only, as the user interface does not read the keys from here. Keep both in sync.
     */
    auto MapReadOnly{[](LInputContext& Context, LStringView Tag, TArray<LKeyChord> Keys)
    {
        Context.MapAction({
            .Tag = LUserInputTag::ToTag(Tag),
            .Flags = EInputActionBits::ReadOnly,
            .Category = EInputActionCategory::Boolean,
            .Triggers = TArray<LInputTrigger>{}
                .reflexive_emplace_back(LInputTrigger::CreateInfo{
                    .TriggerFlags = EInputActionTriggerBits::Triggered,
                    .DefaultKeys = std::move(Keys),
                    }),
            .Callback = [](LInputCallback const&, LInputActionValue&) -> LOnUserInputActionResult { return {}; },
            });
    }};

    if (LInputContext* Context{Registry.RegisterContext("EditorWorldViewer")})
    {
        /* See #WEditorWorldViewer::Tick. Always works, even if the user input contexts are compromised. */
        MapReadOnly(*Context, "ReleaseMouse", {LKeyChord{Key(ELogicalKey::F1), EModBits::Shift}});
        /* See #WEditorWorldViewer::OnKeyEventFocused. */
        MapReadOnly(*Context, "SelectTool", {Key(ELogicalKey::Q)});
        MapReadOnly(*Context, "TranslateTool", {Key(ELogicalKey::W)});
        MapReadOnly(*Context, "RotateTool", {Key(ELogicalKey::E)});
        MapReadOnly(*Context, "ScaleTool", {Key(ELogicalKey::R)});
        MapReadOnly(*Context, "GizmoTool", {Key(ELogicalKey::T)});
        MapReadOnly(*Context, "FocusSelection", {Key(ELogicalKey::F)});
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [EditorWorldViewer] user input context.")
    }

    if (LInputContext* Context{Registry.RegisterContext("TextField")})
    {
        /* Text fields resolve letter shortcuts by the letter that a key produces in the keyboard layout of the user. */
        auto Letter{[&Frontend, &Key](ELogicalKey Fallback, char Lower) -> LPhysicalKey
        {
            for (auto Idx{static_cast<i32>(ELogicalKey::A)}; Idx <= static_cast<i32>(ELogicalKey::Z); ++Idx)
            {
                if (auto Candidate{Frontend.GetPhysicalKey(static_cast<ELogicalKey>(Idx))})
                {
                    /* GLFW_KEY_UNKNOWN resolves the name by the scancode. */
                    if (auto Name{Frontend.Glfw_GetPhysicalKeyLocalizedRepr(*Candidate, -1)};
                        Name && Name->size() == 1uz && (Name->front() == Lower || Name->front() == Lower - 'a' + 'A'))
                    {
                        return *Candidate;
                    }
                }
            }
            return Key(Fallback);
        }};
        const EModFlags Ctrl{EModBits::Control};
        const EModFlags Shift{EModBits::Shift};

        /* See #WEditableTextButton::HandleEditingKey. */
        MapReadOnly(*Context, "SelectAll", {LKeyChord{Letter(ELogicalKey::A, 'a'), Ctrl}});
        MapReadOnly(*Context, "Copy", {LKeyChord{Letter(ELogicalKey::C, 'c'), Ctrl}, LKeyChord{Key(ELogicalKey::Insert), Ctrl}});
        MapReadOnly(*Context, "Cut", {LKeyChord{Letter(ELogicalKey::X, 'x'), Ctrl}, LKeyChord{Key(ELogicalKey::Delete), Shift}});
        MapReadOnly(*Context, "Paste", {LKeyChord{Letter(ELogicalKey::V, 'v'), Ctrl}, LKeyChord{Key(ELogicalKey::Insert), Shift}});
        MapReadOnly(*Context, "Undo", {LKeyChord{Letter(ELogicalKey::Z, 'z'), Ctrl}});
        MapReadOnly(*Context, "Redo", {LKeyChord{Letter(ELogicalKey::Y, 'y'), Ctrl}, LKeyChord{Letter(ELogicalKey::Z, 'z'), Ctrl|Shift}});
        MapReadOnly(*Context, "Commit", {Key(ELogicalKey::Enter), Key(ELogicalKey::NumPadEnter)});
        MapReadOnly(*Context, "Cancel", {Key(ELogicalKey::Escape)});
    }
    else
    {
        LOG_WARNING(LogUserInput, "Failed to register [TextField] user input context.")
    }

    return;
}
