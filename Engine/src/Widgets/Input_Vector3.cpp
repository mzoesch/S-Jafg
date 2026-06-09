// Copyright mzoesch. All rights reserved.

#include "Widgets/Input_Vector3.h"
#include "Nodes/EditableTextButton.h"
#include "Nodes/Text.h"
#include "Platform/SurfaceGlfw3.h"
#include "Serialization/StringStatements.h"
#include "User/UserPreferences.h"
#include "Engine/Engine.h"

void Jafg::WInput_Vector3::Construct()
{
    Super::Construct();

    check(!std::holds_alternative<std::monostate>(this->Vector))

    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto CreateButton([this, &Prefs](EPart Part, LColor const& Tint, WEditableTextButton** Button)
    {
        return NewNode(this->GetViewport()).Class<WEditableTextButtonIconizedLeft>().SaveTo(Button)
            .Anchor(EAnchor::HFill)
            .MinDesiredSize({70_spt, 0})
            .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
            .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
            .TextCutoff(ETextCutoff::Cutoff)
            .InAllBrushes<&LBoxBrush::Padding>(LPadding{2_spt, 0, 4, 0})
            .Icon("Icons/Jafg.AxisHint")
            .IconTint(Tint)
            .IconAlignment(LIconBrush::Align::Left)
            .IconMinSize(2_spt)
            .IconInwardsPadding(4_spt)
            .OnContentCommitted([this](WEditableTextButton&, LString const&, ETextCommit){ this->OnVectorDataChanged(false); })
            .OnContentChanged([this, Part](WEditableTextButton&, LString const& Content){ this->OnTextButtonContentChanged(Part, Content); })
            .OnCursorEnter([](auto&&...){ return LNodeReply{ECursor::ResizeEW}; })
            .OnKeyEventFocused([this, Part](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)) && Info.CursorLocation)
                {
                    Info.Viewport.EmplaceUntil<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton),
                    [this, Part, LastLocation=Info.CursorLocation->x] mutable
                    {
                        auto& Surface{this->GetViewport().GetSurface()};

                        if (!Surface.HasMouseLocation())
                        {
                            return true;
                        }

                        auto Delta{(Surface.GetMouseLocation()->x - LastLocation) * 0.01};
                        LastLocation = Surface.GetMouseLocation()->x;
                        if (Delta != 0.0f)
                        {
                            this->GetViewport().GetSurface().SetInputMode(EInputModeBits::HideMouseCursor);
                            if (std::holds_alternative<SignedVector>(this->Vector))
                            {
                                auto Vec{std::get<SignedVector>(this->Vector)};
                                this->ProjVecL(Part, Vec) += static_cast<SignedVector::value_type>(Delta);
                                this->Set(Vec);
                            }
                            else if (std::holds_alternative<UnsignedVector>(this->Vector))
                            {
                                auto Vec{std::get<UnsignedVector>(this->Vector)};
                                this->ProjVecL(Part, Vec) += static_cast<UnsignedVector::value_type>(Delta);
                                this->Set(Vec);
                            }
                            else if (std::holds_alternative<FloatingVector>(this->Vector))
                            {
                                auto Vec{std::get<FloatingVector>(this->Vector)};
                                this->ProjVecL(Part, Vec) += static_cast<FloatingVector::value_type>(Delta);
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

    WEditableTextButton *ButtonX, *ButtonY, *ButtonZ;
    this->AddChild(CreateButton(EPart::X, *Prefs.EditorAxisTintX, &ButtonX).Unique());
    this->AddChild(CreateButton(EPart::Y, *Prefs.EditorAxisTintY, &ButtonY).Unique());
    this->AddChild(CreateButton(EPart::Z, *Prefs.EditorAxisTintZ, &ButtonZ).Unique());

    if (std::holds_alternative<SignedVector>(this->Vector))
    {
        ButtonX->ContentPredicate = serde::IsIntegral;
        ButtonY->ContentPredicate = serde::IsIntegral;
        ButtonZ->ContentPredicate = serde::IsIntegral;
        ButtonX->SetContent(this->FormatAxis(EPart::X, std::get<SignedVector>(this->Vector)));
        ButtonY->SetContent(this->FormatAxis(EPart::Y, std::get<SignedVector>(this->Vector)));
        ButtonZ->SetContent(this->FormatAxis(EPart::Z, std::get<SignedVector>(this->Vector)));
    }
    else if (std::holds_alternative<UnsignedVector>(this->Vector))
    {
        ButtonX->ContentPredicate = serde::IsUIntegral;
        ButtonY->ContentPredicate = serde::IsUIntegral;
        ButtonZ->ContentPredicate = serde::IsUIntegral;
        ButtonX->SetContent(this->FormatAxis(EPart::X, std::get<UnsignedVector>(this->Vector)));
        ButtonY->SetContent(this->FormatAxis(EPart::Y, std::get<UnsignedVector>(this->Vector)));
        ButtonZ->SetContent(this->FormatAxis(EPart::Z, std::get<UnsignedVector>(this->Vector)));
    }
    else if (std::holds_alternative<FloatingVector>(this->Vector))
    {
        ButtonX->ContentPredicate = serde::IsNumeric;
        ButtonY->ContentPredicate = serde::IsNumeric;
        ButtonZ->ContentPredicate = serde::IsNumeric;
        ButtonX->SetContent(this->FormatAxis(EPart::X, std::get<FloatingVector>(this->Vector)));
        ButtonY->SetContent(this->FormatAxis(EPart::Y, std::get<FloatingVector>(this->Vector)));
        ButtonZ->SetContent(this->FormatAxis(EPart::Z, std::get<FloatingVector>(this->Vector)));
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

void Jafg::WInput_Vector3::SetInputEnabled(bool bEnabled)
{
#if JAFG_DO_CHECKS
    std::size_t Count{};
#endif /* JAFG_DO_CHECKS */

    for (auto& Child : this->GetChildren())
    {
        if (auto* Btn{Child->As<WEditableTextButton>()})
        {
            Btn->SetEnabled(bEnabled);
            checkCode(++Count)
        }
    }

    check(Count == 3)
    return;
}

void Jafg::WInput_Vector3::OnTextButtonContentChanged(EPart Part, LString const& NewValue)
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
            if (Part == EPart::X)
            {
                serde::FromString(&Vec.x, NewValue);
            }
            else if (Part == EPart::Y)
            {
                serde::FromString(&Vec.y, NewValue);
            }
            else
            {
                serde::FromString(&Vec.z, NewValue);
            }
            this->Vector.emplace<SignedVector>(Vec);
        }
        else if (std::holds_alternative<UnsignedVector>(this->Vector))
        {
            auto& Vec{std::get<UnsignedVector>(this->Vector)};
            if (Part == EPart::X)
            {
                serde::FromString(&Vec.x, NewValue);
            }
            else if (Part == EPart::Y)
            {
                serde::FromString(&Vec.y, NewValue);
            }
            else
            {
                serde::FromString(&Vec.z, NewValue);
            }
            this->Vector.emplace<UnsignedVector>(Vec);
        }
        else if (std::holds_alternative<FloatingVector>(this->Vector))
        {
            if (NewValue == "-")
            {
                return;
            }

            auto& Vec{std::get<FloatingVector>(this->Vector)};
            if (Part == EPart::X)
            {
                serde::FromString(&Vec.x, NewValue);
            }
            else if (Part == EPart::Y)
            {
                serde::FromString(&Vec.y, NewValue);
            }
            else
            {
                serde::FromString(&Vec.z, NewValue);
            }
            this->Vector.emplace<FloatingVector>(Vec);
        }
        else
        {
            std::unreachable();
        }

        this->OnVectorChanged.InvokeIfBound(*this);
    }

    return;
}

void Jafg::WInput_Vector3::OnVectorDataChanged(bool bCallEvent)
{
    check(!this->bIgnoreChangeEvents)
    this->bIgnoreChangeEvents = true;
    algo::raii_leave _{[this]{ this->bIgnoreChangeEvents = false; }};

#if JAFG_DO_CHECKS
    std::size_t Count{};
#endif /* JAFG_DO_CHECKS */

    auto Part{EPart::X};
    for (auto& Child : this->GetChildren())
    {
        if (auto* Btn{Child->As<WEditableTextButton>()})
        {
            if (std::holds_alternative<SignedVector>(this->Vector))
            {
                auto Vec{std::get<SignedVector>(this->Vector)};
                Btn->SetContent(this->FormatAxis(Part, Vec));
            }
            else if (std::holds_alternative<UnsignedVector>(this->Vector))
            {
                auto Vec{std::get<UnsignedVector>(this->Vector)};
                Btn->SetContent(this->FormatAxis(Part, Vec));
            }
            else if (std::holds_alternative<FloatingVector>(this->Vector))
            {
                auto Vec{std::get<FloatingVector>(this->Vector)};
                Btn->SetContent(this->FormatAxis(Part, Vec));
            }
            else
            {
                std::unreachable();
            }

            if (Part == EPart::X)
            {
                Part = EPart::Y;
            }
            else if (Part == EPart::Y)
            {
                Part = EPart::Z;
            }

            checkCode(++Count);
        }
    }

    check(Count == 3 || this->_IsGarbage())

    if (bCallEvent)
    {
        this->OnVectorChanged.InvokeIfBound(*this);
    }

    return;
}
