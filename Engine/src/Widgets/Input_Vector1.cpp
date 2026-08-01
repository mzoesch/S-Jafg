// Copyright mzoesch. All rights reserved.

#include "Widgets/Input_Vector1.h"
#include "Nodes/EditableTextButton.h"
#include "Nodes/Text.h"
#include "Platform/SurfaceGlfw3.h"
#include "Serialization/StringStatements.h"
#include "User/UserPreferences.h"
#include "Engine/Engine.h"

void Jafg::WInput_Vector1::Construct()
{
    Super::Construct();

    check(!std::holds_alternative<std::monostate>(this->Vector))

    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto CreateButton([this, &Prefs](WEditableTextButton** Button)
    {
        return NewNode(this->GetViewport()).Class<WEditableTextButton>().SaveTo(Button)
            .Anchor(EAnchor::HFill)
            .MinDesiredSize({70_spt, 0})
            .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
            .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
            .TextCutoff(ETextCutoff::Cutoff)
            .OnContentCommitted([this](WEditableTextButton&, LString const&, ETextCommit){ this->OnVectorDataChanged(false); })
            .OnContentChanged([this](WEditableTextButton&, LString const& Content){ this->OnTextButtonContentChanged(Content); })
            .OnCursorEnter([](auto&&...){ return LNodeReply{ECursor::ResizeEW}; })
            .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)) && Info.CursorLocation)
                {
                    Info.Viewport.EmplaceUntil<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton),
                    [this, LastLocation=Info.CursorLocation->x](auto&&...) mutable
                    {
                        auto& Surface{this->GetViewport().GetSurface()};

                        if (!Surface.HasMouseLocation())
                        {
                            return true;
                        }

                        auto Delta{(Surface.GetMouseLocation()->x - LastLocation) * this->Acceleration};
                        LastLocation = Surface.GetMouseLocation()->x;
                        if (Delta != 0.0f)
                        {
                            this->GetViewport().GetSurface().SetInputMode(EInputModeBits::HideMouseCursor);
                            if (std::holds_alternative<SignedVector>(this->Vector))
                            {
                                auto Vec{std::get<SignedVector>(this->Vector)};
                                Vec += static_cast<SignedVector>(Delta);
                                this->Set(Vec);
                            }
                            else if (std::holds_alternative<UnsignedVector>(this->Vector))
                            {
                                auto Vec{std::get<UnsignedVector>(this->Vector)};
                                Vec += static_cast<UnsignedVector>(Delta);
                                this->Set(Vec);
                            }
                            else if (std::holds_alternative<FloatingVector>(this->Vector))
                            {
                                auto Vec{std::get<FloatingVector>(this->Vector)};
                                Vec += static_cast<FloatingVector>(Delta);
                                this->Set(Vec);
                            }
                            else
                            {
                                std::unreachable();
                            }
                        }
                        return false;
                    }, [this]{ this->GetViewport().GetSurface().SetInputMode(EInputModeBits::ShowMouseCursor); });
                }
                return LNodeReply::Unhandled();
            })
            .OnCursorLeave([this](auto&&...){ this->GetViewport().GetSurface()._SetMouseCursor(ECursor::Default); })
            .Enabled(this->bInitialEnabledState.has_value() ? *this->bInitialEnabledState : true);
    });

    WEditableTextButton *Button;
    this->AddChild(CreateButton(&Button).Unique());

    if (std::holds_alternative<SignedVector>(this->Vector))
    {
        Button->ContentPredicate = serde::IsIntegral;
        Button->SetContent(this->FormatAxis(std::get<SignedVector>(this->Vector)));
    }
    else if (std::holds_alternative<UnsignedVector>(this->Vector))
    {
        Button->ContentPredicate = serde::IsUIntegral;
        Button->SetContent(this->FormatAxis(std::get<UnsignedVector>(this->Vector)));
    }
    else if (std::holds_alternative<FloatingVector>(this->Vector))
    {
        Button->ContentPredicate = serde::IsNumeric;
        Button->SetContent(this->FormatAxis(std::get<FloatingVector>(this->Vector)));
    }
    else
    {
        std::unreachable();
    }

    if (this->bInitialEnabledState)
    {
        this->SetInputEnabled(*this->bInitialEnabledState);
        this->bInitialEnabledState.reset();
    }

    return;
}

void Jafg::WInput_Vector1::SetInputEnabled(bool bEnabled)
{
#if JAFG_DO_CHECKS
    std::size_t Count{};
#endif /* JAFG_DO_CHECKS */

    for (auto& Child: this->GetChildren())
    {
        if (auto* Btn{Child->As<WEditableTextButton>()})
        {
            Btn->SetEnabled(bEnabled);
            checkCode(++Count)
        }
    }

    check(Count == 1)
}

void Jafg::WInput_Vector1::OnTextButtonContentChanged(LString const& NewValue)
{
    check(serde::IsNumeric(NewValue))

    if (!this->bIgnoreChangeEvents && !NewValue.empty())
    {
        if (std::holds_alternative<SignedVector>(this->Vector))
        {
            if (NewValue == "-")
            {
                return;
            }

            auto& Vec{std::get<SignedVector>(this->Vector)};
            serde::FromString(&Vec, NewValue);
            this->Vector.emplace<SignedVector>(Vec);
        }
        else if (std::holds_alternative<UnsignedVector>(this->Vector))
        {
            auto& Vec{std::get<UnsignedVector>(this->Vector)};
            serde::FromString(&Vec, NewValue);
            this->Vector.emplace<UnsignedVector>(Vec);
        }
        else if (std::holds_alternative<FloatingVector>(this->Vector))
        {
            if (NewValue == "-")
            {
                return;
            }

            auto& Vec{std::get<FloatingVector>(this->Vector)};
            serde::FromString(&Vec, NewValue);
            this->Vector.emplace<FloatingVector>(Vec);
        }
        else
        {
            std::unreachable();
        }

        this->OnVectorChanged.InvokeIfBound(*this);
    }
}

void Jafg::WInput_Vector1::OnVectorDataChanged(bool bCallEvent)
{
    check(!this->bIgnoreChangeEvents)
    this->bIgnoreChangeEvents = true;
    algo::raii_leave _{[this]{ this->bIgnoreChangeEvents = false; }};

#if JAFG_DO_CHECKS
    std::size_t Count{};
#endif /* JAFG_DO_CHECKS */

    for (auto& Child: this->GetChildren())
    {
        if (auto* Btn{Child->As<WEditableTextButton>()})
        {
            if (std::holds_alternative<SignedVector>(this->Vector))
            {
                auto Vec{std::get<SignedVector>(this->Vector)};
                Btn->SetContent(this->FormatAxis(Vec));
            }
            else if (std::holds_alternative<UnsignedVector>(this->Vector))
            {
                auto Vec{std::get<UnsignedVector>(this->Vector)};
                Btn->SetContent(this->FormatAxis(Vec));
            }
            else if (std::holds_alternative<FloatingVector>(this->Vector))
            {
                auto Vec{std::get<FloatingVector>(this->Vector)};
                Btn->SetContent(this->FormatAxis(Vec));
            }
            else
            {
                std::unreachable();
            }

            checkCode(++Count)
        }
    }

    check(Count == 1 || this->_IsGarbage())

    if (bCallEvent)
    {
        this->OnVectorChanged.InvokeIfBound(*this);
    }
}
