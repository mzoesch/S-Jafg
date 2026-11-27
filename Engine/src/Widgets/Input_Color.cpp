// Copyright mzoesch. All rights reserved.

#include "Widgets/Input_Color.h"
#include "Nodes/EditableTextButton.h"
#include "Nodes/Text.h"
#include "Framework/SurfaceGlfw3.h"
#include "Serialization/StringStatements.h"
#include "User/UserPreferences.h"
#include "Engine/Engine.h"

void Jafg::WInput_Color::Construct()
{
    Super::Construct();

    check(!std::holds_alternative<std::monostate>(this->Vector))

    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto CreateButton([this, &Prefs](EPart Part, LColor const& Tint, WEditableTextButton** Button)
    {
        return NewNode(this->GetViewport()).Class<WEditableTextButtonIconizedLeft>().SaveTo(Button)
            .Anchor(EAnchor::HFill)
            .MinDesiredSize({70_spt, 0})
            .Palette(*Prefs.InputPaletteSolid)
            .TextCutoff(ETextCutoff::Cutoff)
            .SelectAllOnFocus(true)
            .InAllBrushes<&LBoxBrush::Padding>(LPadding{2_spt, 0, 4, 0})
            .Icon({JafgInfo, "Icons/AxisHint"})
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
                    [this, Part, LastLocation=Info.CursorLocation->x](auto&&...) mutable
                    {
                        auto& Surface{this->GetViewport().GetSurface()};

                        if (!Surface.HasMouseLocation())
                        {
                            return true;
                        }

                        auto Delta{(Surface.GetMouseLocation()->x - LastLocation) * this->Range.GetDragScale(0.01)};
                        LastLocation = Surface.GetMouseLocation()->x;
                        if (Delta != 0.0f)
                        {
                            this->GetViewport().GetSurface().SetInputMode(EInputModeBits::HideMouseCursor);
                            if (std::holds_alternative<LColor>(this->Vector))
                            {
                                auto Vec{std::get<LColor>(this->Vector)};
                                auto const Start{this->ProjVec(Part, Vec)};
                                this->ProjVecL(Part, Vec) = this->Range.ClampDragged(Start, static_cast<LColor::value_type>(Start + static_cast<LColor::value_type>(Delta)));
                                this->Set(Vec);
                            }
                            else if (std::holds_alternative<LLinearColor>(this->Vector))
                            {
                                auto Vec{std::get<LLinearColor>(this->Vector)};
                                auto const Start{this->ProjVec(Part, Vec)};
                                this->ProjVecL(Part, Vec) = this->Range.ClampDragged(Start, static_cast<LLinearColor::value_type>(Start + static_cast<LLinearColor::value_type>(Delta)));
                                this->Set(Vec);
                            }
                            else if (std::holds_alternative<LLinearColor3>(this->Vector))
                            {
                                auto Vec{std::get<LLinearColor3>(this->Vector)};
                                auto const Start{this->ProjVec(Part, Vec)};
                                this->ProjVecL(Part, Vec) = this->Range.ClampDragged(Start, static_cast<LLinearColor3::value_type>(Start + static_cast<LLinearColor3::value_type>(Delta)));
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

    WEditableTextButton* ButtonR{};
    WEditableTextButton* ButtonG{};
    WEditableTextButton* ButtonB{};
    WEditableTextButton* ButtonA{};
    this->AddChild(CreateButton(EPart::R, Colors::Crimson, &ButtonR).Unique());
    this->AddChild(CreateButton(EPart::G, Colors::LimeGreen, &ButtonG).Unique());
    this->AddChild(CreateButton(EPart::B, Colors::DeepSkyBlue, &ButtonB).Unique());
    if (std::holds_alternative<LLinearColor>(this->Vector) || std::holds_alternative<LColor>(this->Vector))
    {
        this->AddChild(CreateButton(EPart::A, Colors::DeepSkyBlue, &ButtonA).Unique());
    }

    check(ButtonR && ButtonG && ButtonB)

    if (std::holds_alternative<LColor>(this->Vector))
    {
        check(ButtonA)

        ButtonR->ContentPredicate = serde::IsUIntegral;
        ButtonG->ContentPredicate = serde::IsUIntegral;
        ButtonB->ContentPredicate = serde::IsUIntegral;
        ButtonA->ContentPredicate = serde::IsUIntegral;
        ButtonR->SetContent(this->FormatAxis(EPart::R, std::get<LColor>(this->Vector)));
        ButtonG->SetContent(this->FormatAxis(EPart::G, std::get<LColor>(this->Vector)));
        ButtonB->SetContent(this->FormatAxis(EPart::B, std::get<LColor>(this->Vector)));
        ButtonA->SetContent(this->FormatAxis(EPart::A, std::get<LColor>(this->Vector)));
    }
    else if (std::holds_alternative<LLinearColor>(this->Vector))
    {
        check(ButtonA)

        ButtonR->ContentPredicate = serde::IsNumeric;
        ButtonG->ContentPredicate = serde::IsNumeric;
        ButtonB->ContentPredicate = serde::IsNumeric;
        ButtonA->ContentPredicate = serde::IsNumeric;
        ButtonR->SetContent(this->FormatAxis(EPart::R, std::get<LLinearColor>(this->Vector)));
        ButtonG->SetContent(this->FormatAxis(EPart::G, std::get<LLinearColor>(this->Vector)));
        ButtonB->SetContent(this->FormatAxis(EPart::B, std::get<LLinearColor>(this->Vector)));
        ButtonA->SetContent(this->FormatAxis(EPart::A, std::get<LLinearColor>(this->Vector)));
    }
    else if (std::holds_alternative<LLinearColor3>(this->Vector))
    {
        ButtonR->ContentPredicate = serde::IsNumeric;
        ButtonG->ContentPredicate = serde::IsNumeric;
        ButtonB->ContentPredicate = serde::IsNumeric;
        check(!ButtonA)
        ButtonR->SetContent(this->FormatAxis(EPart::R, std::get<LLinearColor3>(this->Vector)));
        ButtonG->SetContent(this->FormatAxis(EPart::G, std::get<LLinearColor3>(this->Vector)));
        ButtonB->SetContent(this->FormatAxis(EPart::B, std::get<LLinearColor3>(this->Vector)));
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

    this->RefreshFill();
    return;
}

void Jafg::WInput_Color::SetInputEnabled(bool bEnabled)
{
    for (auto& Child: this->GetChildren())
    {
        if (auto* Btn{Child->As<WEditableTextButton>()})
        {
            Btn->SetEnabled(bEnabled);
        }
    }
}

void Jafg::WInput_Color::OnTextButtonContentChanged(EPart Part, LString const& NewValue)
{
    check(serde::IsNumeric(NewValue))

    if (!this->bIgnoreChangeEvents && !NewValue.empty())
    {
        if (std::holds_alternative<LColor>(this->Vector))
        {
            if (NewValue == "-")
            {
                return;
            }

            auto& Vec{std::get<LColor>(this->Vector)};
            if (Part == EPart::R)
            {
                serde::from_string(&Vec.r, NewValue);
            }
            else if (Part == EPart::G)
            {
                serde::from_string(&Vec.g, NewValue);
            }
            else if (Part == EPart::B)
            {
                serde::from_string(&Vec.b, NewValue);
            }
            else if (Part == EPart::A)
            {
                serde::from_string(&Vec.a, NewValue);
            }
            /* Clamps only the value. The text shows what was typed until it is committed. */
            this->Vector.emplace<LColor>(this->ClampColor(Vec));
        }
        else if (std::holds_alternative<LLinearColor>(this->Vector))
        {
            if (NewValue == "-")
            {
                return;
            }

            auto& Vec{std::get<LLinearColor>(this->Vector)};
            if (Part == EPart::R)
            {
                serde::from_string(&Vec.r, NewValue);
            }
            else if (Part == EPart::G)
            {
                serde::from_string(&Vec.g, NewValue);
            }
            else if (Part == EPart::B)
            {
                serde::from_string(&Vec.b, NewValue);
            }
            else if (Part == EPart::A)
            {
                serde::from_string(&Vec.a, NewValue);
            }
            /* Clamps only the value. The text shows what was typed until it is committed. */
            this->Vector.emplace<LLinearColor>(this->ClampColor(Vec));
        }
        else if (std::holds_alternative<LLinearColor3>(this->Vector))
        {
            if (NewValue == "-")
            {
                return;
            }

            auto& Vec{std::get<LLinearColor3>(this->Vector)};
            if (Part == EPart::R)
            {
                serde::from_string(&Vec.r, NewValue);
            }
            else if (Part == EPart::G)
            {
                serde::from_string(&Vec.g, NewValue);
            }
            else if (Part == EPart::B)
            {
                serde::from_string(&Vec.b, NewValue);
            }
            else if (Part == EPart::A)
            {
                check(Part != EPart::A)
            }
            /* Clamps only the value. The text shows what was typed until it is committed. */
            this->Vector.emplace<LLinearColor3>(this->ClampColor(Vec));
        }
        else
        {
            std::unreachable();
        }

        this->RefreshFill();
        this->OnVectorChanged.InvokeIfBound(*this);
    }

    return;
}

void Jafg::WInput_Color::OnVectorDataChanged(bool bCallEvent)
{
    check(!this->bIgnoreChangeEvents)
    this->bIgnoreChangeEvents = true;
    algo::raii_leave _{[this]{ this->bIgnoreChangeEvents = false; }};

    auto Part{EPart::R};
    for (auto& Child : this->GetChildren())
    {
        if (auto* Btn{Child->As<WEditableTextButton>()})
        {
            if (std::holds_alternative<LLinearColor>(this->Vector))
            {
                auto Vec{std::get<LLinearColor>(this->Vector)};
                Btn->SetContent(this->FormatAxis(Part, Vec));
            }
            else if (std::holds_alternative<LLinearColor3>(this->Vector))
            {
                auto Vec{std::get<LLinearColor3>(this->Vector)};
                Btn->SetContent(this->FormatAxis(Part, Vec));
            }
            else if (std::holds_alternative<LColor>(this->Vector))
            {
                auto Vec{std::get<LColor>(this->Vector)};
                Btn->SetContent(this->FormatAxis(Part, Vec));
            }
            else
            {
                std::unreachable();
            }

            if (Part == EPart::R)
            {
                Part = EPart::G;
            }
            else if (Part == EPart::G)
            {
                Part = EPart::B;
            }
            else if (Part == EPart::B)
            {
                Part = EPart::A;
            }
        }
    }

    this->RefreshFill();
    if (bCallEvent)
    {
        this->OnVectorChanged.InvokeIfBound(*this);
    }

    return;
}

void Jafg::WInput_Color::RefreshFill() noexcept
{
    auto Part{EPart::R};
    for (auto& Child: this->GetChildren())
    {
        if (auto* Btn{Child->As<WEditableTextButton>()})
        {
            Btn->FillFraction = std::visit([this, Part](auto const& Value) -> std::optional<f32>
            {
                if constexpr (std::is_same_v<std::remove_cvref_t<decltype(Value)>, std::monostate>)
                {
                    return {};
                }
                else
                {
                    return this->Range.GetFraction(static_cast<f64>(this->ProjVec(Part, Value)));
                }
            }, this->Vector);
            Part = Part == EPart::R ? EPart::G : Part == EPart::G ? EPart::B : EPart::A;
        }
    }
}
