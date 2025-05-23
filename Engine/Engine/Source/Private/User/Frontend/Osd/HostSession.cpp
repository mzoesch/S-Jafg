// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/HostSession.h"
#include "Widgets/Button.h"
#include "Widgets/EditableTextBlock.h"
#include "Widgets/Switcher.h"
#include "Widgets/Region.h"
#include "Widgets/ScrollRegion.h"
#include "Widgets/VRegion.h"
#include "Widgets/HRegion.h"
#include "Widgets/Spacer.h"
#include "Widgets/Blueprint/CommonMenuTabBar.h"

namespace
{

struct LHostSessionScreenData final : public Jafg::LWidgetNodeData
{
    Jafg::WHostSessionScreen* Screen { nullptr };
};

} /* ~Namespace <Anonymous> */

void Jafg::WHostSessionScreen::Construct()
{
    Super::Construct();

    LHostSessionScreenData DataOld;
    DataOld.DerivedClass = WHostSessionScreen_Old::StaticClass()->GetName();
    DataOld.Screen = this;

    LHostSessionScreenData DataNew;
    DataNew.DerivedClass = WHostSessionScreen_New::StaticClass()->GetName();
    DataNew.Screen = this;

    MakeRootNode(WSwitcher).SaveTo(&this->Switcher)
        .Anchor(EAnchor::Fill)
    [
        NewNode(WHostSessionScreen_Old).SaveTo(&this->OldScreen)
            .Data(&DataOld)
        +
        NewNode(WHostSessionScreen_New).SaveTo(&this->NewScreen)
            .Data(&DataNew)
    ]
    FinishWidgetStyling()

    this->Switcher->SetActiveWidget(this->NewScreen);

    return;
}

void Jafg::WHostSessionScreen::ShowOldScreen()
{
    check( this->Switcher )
    check( this->OldScreen )

    this->Switcher->SetActiveWidget(this->OldScreen);

    return;
}

void Jafg::WHostSessionScreen::ShowNewScreen()
{
    check( this->Switcher )
    check( this->NewScreen )

    this->Switcher->SetActiveWidget(this->NewScreen);

    return;
}

Jafg::WHostSessionScreen_New::WHostSessionScreen_New(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    return;
}

void Jafg::WHostSessionScreen_New::Construct()
{
    this->SetTint({40, 39, 49});

    NewNode(WCommonMenuTabBar).SaveTo(&this->InternalTabBar)
        .DisallowNone()
        .DefaultIndex(0)
    [
        LTabBarTabDescriptor()
            .Identifier("General")
            .PanelWidgetClass(WVRegion::StaticClass())
            .Callback(LOnTabBarLoaded::CreateMemberFunction(this, &WHostSessionScreen_New::OnLoad_General))
        +
        LTabBarTabDescriptor()
            .Identifier("Multiplayer")
            .PanelWidgetClass(WVRegion::StaticClass())
            .Callback(LOnTabBarLoaded::CreateMemberFunction(this, &WHostSessionScreen_New::OnLoad_Multiplayer))
        +
        LTabBarTabDescriptor()
            .Identifier("World Generation")
            .PanelWidgetClass(WVRegion::StaticClass())
            .Callback(LOnTabBarLoaded::CreateMemberFunction(this, &WHostSessionScreen_New::OnLoad_WorldGeneration))
        +
        LTabBarTabDescriptor()
            .Identifier("Policies")
            .PanelWidgetClass(WVRegion::StaticClass())
            .Callback(LOnTabBarLoaded::CreateMemberFunction(this, &WHostSessionScreen_New::OnLoad_Policies))
        +
        LTabBarTabDescriptor()
            .Identifier("Advanced")
            .PanelWidgetClass(WVRegion::StaticClass())
            .Callback(LOnTabBarLoaded::CreateMemberFunction(this, &WHostSessionScreen_New::OnLoad_Advanced))
    ]
    ;

    this->AddChild(this->InternalTabBar);

    Super::Construct();

    return;
}

bool Jafg::WHostSessionScreen_New::AddData(const LWidgetNodeData* InData)
{
    const bool bSuper = Super::AddData(InData);
    if (InData->DerivedClass != WHostSessionScreen_New::StaticClass()->GetName())
    {
        return bSuper;
    }

    const LHostSessionScreenData* Data = static_cast<const LHostSessionScreenData*>(InData);
    this->Owner = Data->Screen;
    check( this->Owner )

    return true;
}

void Jafg::WHostSessionScreen_New::OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility)
{
    Super::OnVisibilityChanged(InOldVisibility, InNewVisibility);

    if (EWidgetVisibility::IsDrawn(InNewVisibility))
    {
        this->InternalTabBar->ResetToDefault();
    }

    return;
}

void Jafg::WHostSessionScreen_New::OnLoad_General(WTabBar* TabBar, WNode* Button, WNode* Panel)
{
    WVRegion* Region = DynamicCast<WVRegion>(Panel);
    jassert( Region )

    Region->SetAnchor(EAnchor::Fill);
    Region->SetPadding({25.0f});

    Region->GetFactory<WVRegion>()
    [
        NewNode(WScrollRegion)
            .Anchor(EAnchor::Fill)
        [
            NewNode(WVRegion)
                .Anchor(EAnchor::Fill)
                .VSpace(5)
            [
                NewNode(WTextBlock)
                    .Anchor(EAnchor::HCenter)
                    .Content("Host Session")
                    .Brush(LTextBlockBrush::Header())
                +
                NewNode(WTextBlock)
                    .Anchor(EAnchor::HLeft)
                    .Content("Name")
                    .Brush(LTextBlockBrush::Header())
                +
                NewNode(WEditableTextBlock)
                    .Anchor(EAnchor::HFill)
                    .MinDesiredSize({45,0})
                    .TextScale(0.5f)
                    .SetPadding({ 5.0f, 4.5f })
                    .Tint({0, 0, 0, 164 })
                    .TextColor(LColor::White)
                +
                NewNode(WTextBlock)
                    .Anchor(EAnchor::HLeft)
                    .Content("Save path")
                    .Brush(LTextBlockBrush::SubHeader())
                +
                NewNode(WSpacer).SetHeight(20.0f)
                +
                NewNode(WHRegion)
                    .Anchor(EAnchor::HFill)
                [
                    NewNode(WTextBlock)
                        .Content("Templates")
                        .Brush(LTextBlockBrush::Body())
                ]
                +
                NewNode(WSpacer).SetHeight(20.0f)
                +
                NewNode(WTextBlock)
                    .Anchor(EAnchor::HLeft)
                    .Content("Player")
                    .Brush(LTextBlockBrush::SubHeader())
                +
                NewNode(WHRegion)
                    .Anchor(EAnchor::HFill)
                [
                    NewNode(WTextBlock)
                        .Content("Perma Death")
                        .Brush(LTextBlockBrush::Body())
                ]
                +
                NewNode(WSpacer).SetHeight(20.0f)
                +
                NewNode(WTextBlock)
                    .Anchor(EAnchor::HLeft)
                    .Content("Enemy")
                    .Brush(LTextBlockBrush::SubHeader())
                +
                NewNode(WHRegion)
                    .Anchor(EAnchor::HFill)
                [
                    NewNode(WTextBlock)
                        .Content("No enemies")
                        .Brush(LTextBlockBrush::Body())
                ]
                +
                NewNode(WHRegion)
                    .Anchor(EAnchor::HFill)
                [
                    NewNode(WTextBlock)
                        .Content("Max Temperament")
                        .Brush(LTextBlockBrush::Body())
                    +
                    NewNode(WTextBlock)
                        .Content("Passive")
                        .Brush(LTextBlockBrush::Body())
                    +
                    NewNode(WTextBlock)
                        .Content("Docile")
                        .Brush(LTextBlockBrush::Body())
                    +
                    NewNode(WTextBlock)
                        .Content("Default")
                        .Brush(LTextBlockBrush::Body())
                ]
            ]
        ]
        +
        NewNode(WHRegion)
        .Anchor(EAnchor::HFill)
        .HSpace(15)
         [
            NewNode(WTextButton)
                .Content("Back")
                .OnPrimaryRelease([this](WButton* Self, const LKeyEvent& InKeyEvent) -> void
                {
                    this->Owner->ShowOldScreen();
                    return;
                })
            +
            NewNode(WSpacer)
                .Anchor(EAnchor::HFill)
            +
            NewNode(WTextButton)
                .Content("Host")
        ]
    ];

    for (LWidgetSlot* Child: Region->GetChildren())
    {
        MakeDeferredWidgetNodeFinal(Child->Content);
    }

    return;
}

void Jafg::WHostSessionScreen_New::OnLoad_Multiplayer(WTabBar* TabBar, WNode* Button, WNode* Panel)
{
    WVRegion* Region = DynamicCast<WVRegion>(Panel);
    jassert( Region )

    Region->SetAnchor(EAnchor::Fill);
    Region->SetTint({40, 39, 49});

    return;
}

void Jafg::WHostSessionScreen_New::OnLoad_WorldGeneration(WTabBar* TabBar, WNode* Button, WNode* Panel)
{
    WVRegion* Region = DynamicCast<WVRegion>(Panel);
    jassert( Region )

    Region->SetAnchor(EAnchor::Fill);
    Region->SetTint({40, 39, 49});

    return;
}

void Jafg::WHostSessionScreen_New::OnLoad_Policies(WTabBar* TabBar, WNode* Button, WNode* Panel)
{
    WVRegion* Region = DynamicCast<WVRegion>(Panel);
    jassert( Region )

    Region->SetAnchor(EAnchor::Fill);
    Region->SetTint({40, 39, 49});

    return;
}

void Jafg::WHostSessionScreen_New::OnLoad_Advanced(WTabBar* TabBar, WNode* Button, WNode* Panel)
{
    WVRegion* Region = DynamicCast<WVRegion>(Panel);
    jassert( Region )

    Region->SetAnchor(EAnchor::Fill);
    Region->SetTint({40, 39, 49});

    return;
}

Jafg::WHostSessionScreen_Old::WHostSessionScreen_Old(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    return;
}

void Jafg::WHostSessionScreen_Old::Construct()
{
    this->SetPadding(25);
    this->SetTint({40, 39, 49});

    WVRegion* Region;
    NewNode(WVRegion).SaveTo(&Region)
        .Anchor(EAnchor::Fill)
        .VSpace(10)
    [
        NewNode(WTextBlock)
            .Anchor(EAnchor::CenterCenter)
            .Content("Host Session")
            .Brush(LTextBlockBrush::Header())
        +
        NewNode(WScrollRegion)
            .Anchor(EAnchor::Fill)
            .Tint({64, 63, 75})
        +
        NewNode(WHRegion)
            .Anchor(EAnchor::HFill)
            .HSpace(15)
        [
            NewNode(WTextButton)
                .Content("Delete Selected")
            +
            NewNode(WTextButton)
                .Content("Edit Selected")
            +
            NewNode(WSpacer)
                .Anchor(EAnchor::HFill)
            +
            NewNode(WTextButton)
                .Content("Host From Selected")
            +
            NewNode(WTextButton)
                .Content("Host From New")
                .OnPrimaryRelease([this](WButton* Self, const LKeyEvent& InKeyEvent) -> void
                {
                    check( this->Owner )
                    this->Owner->ShowNewScreen();
                    return;
                })
        ]
    ];
    this->AddChild(Region);

    Super::Construct();

    return;
}

bool Jafg::WHostSessionScreen_Old::AddData(const LWidgetNodeData* InData)
{
    const bool bSuper = Super::AddData(InData);
    if (InData->DerivedClass != WHostSessionScreen_Old::StaticClass()->GetName())
    {
        return bSuper;
    }

    const LHostSessionScreenData* Data = static_cast<const LHostSessionScreenData*>(InData);
    this->Owner = Data->Screen;
    check( this->Owner )

    return true;
}
