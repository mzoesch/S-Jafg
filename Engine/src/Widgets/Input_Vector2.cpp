// Copyright mzoesch. All rights reserved.

#include "Widgets/Input_Vector2.h"
#include "Nodes/EditableTextButton.h"
#include "Nodes/Text.h"
#include "Serialization/StringStatements.h"
#include "User/UserPreferences.h"

void Jafg::WInput_Vector2::Construct()
{
    Super::Construct();

    check(!std::holds_alternative<std::monostate>(this->Vector))

    auto& Prefs{GetSingleton<JUserPreferences>()};

    WEditableTextButton *ButtonX, *ButtonY;
    BeginStyling(*this).StaticRoot<WEditableTextButton>().SaveTo(&ButtonX)
        .MinDesiredSize({64_spt, 0})
        .MaxDesiredSize({64_spt, 0})
        .Padding({2_spt, 0})
        .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
        .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
        .OnContentChanged([this](WEditableTextButton&, LString const& Content){ this->OnTextButtonContentChanged(EPart::X, Content); })
        .Enabled(this->bInitialEnabledState.has_value() ? *this->bInitialEnabledState : true);
    BeginStyling(*this).StaticRoot<WEditableTextButton>().SaveTo(&ButtonY)
        .MinDesiredSize({64_spt, 0})
        .MaxDesiredSize({64_spt, 0})
        .Padding({2_spt, 0})
        .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
        .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
        .OnContentChanged([this](WEditableTextButton&, LString const& Content){ this->OnTextButtonContentChanged(EPart::Y, Content); })
        .Enabled(this->bInitialEnabledState.has_value() ? *this->bInitialEnabledState : true);

    if (std::holds_alternative<LVec2i64>(this->Vector))
    {
        ButtonX->ContentPredicate = serde::IsIntegral;
        ButtonY->ContentPredicate = serde::IsIntegral;
        ButtonX->SetContent(std::to_string(std::get<LVec2i64>(this->Vector).x));
        ButtonY->SetContent(std::to_string(std::get<LVec2i64>(this->Vector).y));
    }
    else if (std::holds_alternative<LVec2u64>(this->Vector))
    {
        ButtonX->ContentPredicate = serde::IsUIntegral;
        ButtonY->ContentPredicate = serde::IsUIntegral;
        ButtonX->SetContent(std::to_string(std::get<LVec2u64>(this->Vector).x));
        ButtonY->SetContent(std::to_string(std::get<LVec2u64>(this->Vector).y));
    }
    else if (std::holds_alternative<LVec2D>(this->Vector))
    {
        ButtonX->ContentPredicate = serde::IsNumeric;
        ButtonY->ContentPredicate = serde::IsNumeric;
        ButtonX->SetContent(std::to_string(std::get<LVec2D>(this->Vector).x));
        ButtonY->SetContent(std::to_string(std::get<LVec2D>(this->Vector).y));
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

void Jafg::WInput_Vector2::SetInputEnabled(bool bEnabled)
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

    check(Count == 2)
    return;
}

void Jafg::WInput_Vector2::OnTextButtonContentChanged(EPart Part, LString const& NewValue)
{
    check(serde::IsNumeric(NewValue))

    if (!this->bIgnoreChangeEvents && !NewValue.empty())
    {
        if (std::holds_alternative<LVec2i64>(this->Vector))
        {
            auto& Vec{std::get<LVec2i64>(this->Vector)};
            if (Part == EPart::X)
            {
                serde::FromString(&Vec.x, NewValue);
            }
            else
            {
                serde::FromString(&Vec.y, NewValue);
            }
            this->Vector.emplace<LVec2i64>(Vec);
        }
        else if (std::holds_alternative<LVec2u64>(this->Vector))
        {
            auto& Vec{std::get<LVec2u64>(this->Vector)};
            if (Part == EPart::X)
            {
                serde::FromString(&Vec.x, NewValue);
            }
            else
            {
                serde::FromString(&Vec.y, NewValue);
            }
            this->Vector.emplace<LVec2u64>(Vec);
        }
        else if (std::holds_alternative<LVec2D>(this->Vector))
        {
            auto& Vec{std::get<LVec2D>(this->Vector)};
            if (Part == EPart::X)
            {
                serde::FromString(&Vec.x, NewValue);
            }
            else
            {
                serde::FromString(&Vec.y, NewValue);
            }
            this->Vector.emplace<LVec2D>(Vec);
        }
        else
        {
            std::unreachable();
        }

        this->OnVectorChanged.InvokeIfBound(*this);
    }

    return;
}

void Jafg::WInput_Vector2::OnVectorDataChanged(bool bCallEvent)
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
            if (std::holds_alternative<LVec2i64>(this->Vector))
            {
                auto Vec{std::get<LVec2i64>(this->Vector)};
                Btn->SetContent(Part == EPart::X ? std::to_string(Vec.x) : std::to_string(Vec.y));
            }
            else if (std::holds_alternative<LVec2u64>(this->Vector))
            {
                auto Vec{std::get<LVec2u64>(this->Vector)};
                Btn->SetContent(Part == EPart::X ? std::to_string(Vec.x) : std::to_string(Vec.y));
            }
            else if (std::holds_alternative<LVec2D>(this->Vector))
            {
                auto Vec{std::get<LVec2D>(this->Vector)};
                Btn->SetContent(Part == EPart::X ? std::to_string(Vec.x) : std::to_string(Vec.y));
            }
            else
            {
                std::unreachable();
            }

            Part = EPart::Y;
            ++Count;
        }
    }

    check(Count == 2)

    if (bCallEvent)
    {
        this->OnVectorChanged.InvokeIfBound(*this);
    }

    return;
}
