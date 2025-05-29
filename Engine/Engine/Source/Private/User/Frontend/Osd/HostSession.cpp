// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/HostSession.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Storage/SaveFunctions.h"
#include "System/Finder.h"
#include "System/Paths.h"
#include "System/EnginePath.h"
#include "User/UserPreferences.h"
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

    this->Switcher->SetActiveWidget(this->OldScreen);

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
    this->SetType(ERegionBrush::Box);
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
                NewNode(WSpacer).Height(20.0f)
                +
                NewNode(WHRegion)
                    .Anchor(EAnchor::HFill)
                [
                    NewNode(WTextBlock)
                        .Content("Templates")
                        .Brush(LTextBlockBrush::Body())
                ]
                +
                NewNode(WSpacer).Height(20.0f)
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
                NewNode(WSpacer).Height(20.0f)
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
                .OnPrimaryRelease([this](WButton* Self, const LKeyEvent& InKeyEvent) -> void
                {
                    if (this->GetOuter()->IsWorld())
                    {
                        this->GetEngine()->Browse(static_cast<LWorld*>(this->GetOuter()), Name_LevelMyWorld.ToString());
                    }
                    else
                    {
                        LOG_ERROR(LogWidgets, "Cannot host, because outer is not a world.")
                    }
                    return;
                })
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

Jafg::WHostSessionScreen_Old_Save::WHostSessionScreen_Old_Save(const Jafg::LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetAnchor(EAnchor::HFill);
    return;
}

void Jafg::WHostSessionScreen_Old_Save::Reload()
{
    if (this->IsSaveValid() == false)
    {
        LOG_WARNING(LogWidgets, "Could not reload save widget because the stored save is invalid.")
        return;
    }

    this->RemoveChildren();

    WRegion* Thumbnail;

    WHRegion* Region;
    NewNode(WHRegion).SaveTo(&Region)
        .Anchor(EAnchor::Fill)
        .Padding({10})
    [
        NewNode(WRegion).SaveTo(&Thumbnail)
            .MinDesiredSize({64})
            .Type(ERegionBrush::Box)
            .Tint(LColor::White)
        +
        NewNode(WVRegion)
            .Anchor(EAnchor::Fill)
        [
            NewNode(WTextBlock)
                .Content(this->Save.DisplayName)
                .Brush(LTextBlockBrush::SubHeader())
            +
            NewNode(WTextBlock)
                .Content(this->Save.Path)
                .Brush(LTextBlockBrush::Body())
                // .Color(LColor::Dark)
        ]
    ]
    FinishWidget(Region);

    if (this->Save.PreviewTexture.IsValid())
    {
        this->Save.PreviewTexture.Upload();
    }
    Thumbnail->SetTexture(&this->Save.PreviewTexture);

    this->AddChild(Region);

    return;
}

Jafg::WHostSessionScreen_Old::WHostSessionScreen_Old(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    return;
}

void Jafg::WHostSessionScreen_Old::Construct()
{
    this->SetPadding(25);
    this->SetType(ERegionBrush::Box);
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
        NewNode(WScrollRegion).SaveTo(&this->SavesRegionContainer)
            .Anchor(EAnchor::Fill)
            .Type(ERegionBrush::OutlineBox)
            .Tint({64, 63, 75})
            .OutlineTint(LColor::Black)
            .Padding({2})
        [
            NewNode(WVRegion).SaveTo(&this->SavesRegion)
                .Anchor(EAnchor::Fill)
                .VSpace(5)
                .Padding({30, 10})
        ]
        +
        NewNode(WHRegion)
            .Anchor(EAnchor::HFill)
            .HSpace(10)
        [
            NewNode(WTextButton).SaveTo(&this->DeleteButton)
                .Content("Delete Selected")
                .Disabled()
            +
            NewNode(WTextButton).SaveTo(&this->EditButton)
                .Content("Edit Selected")
                .Disabled()
            +
            NewNode(WSpacer)
                .Anchor(EAnchor::HFill)
            +
            NewNode(WTextButton).SaveTo(&this->HostButton)
                .Content("Host From Selected")
                .Disabled()
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

    this->RefetchSaves();

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

void Jafg::WHostSessionScreen_Old::RefetchSaves()
{
    check( this->SavesRegion )
    this->SavesRegion->RemoveChildren();
    this->SelectedSaveIndex = -1;

    if (this->DeleteButton)
    {
        this->DeleteButton->SetEnabled(false);
    }
    if (this->EditButton)
    {
        this->EditButton->SetEnabled(false);
    }
    if (this->HostButton)
    {
        this->HostButton->SetEnabled(false);
    }

    this->RefetchSavesImpl();

    if (this->FetchedSaves.IsEmpty())
    {
        WTextBlock* Text;
        NewNode(WTextBlock).SaveTo(&Text)
            .Anchor(EAnchor::Fill)
            .Brush(LTextBlockBrush::SubHeader())
            .HAlign(ETextHAlign::Center)
            .VAlign(ETextVAlign::Center)
            .Content("No sessions found. Select \"Host From New\" to create a new sessions.")
        FinishWidget(Text);

        this->SavesRegion->AddChild(Text);

        return;
    }

    for (LFetchedSave& Save : this->FetchedSaves)
    {
        WHostSessionScreen_Old_Save* SaveWidget;
            NewNode(WHostSessionScreen_Old_Save).SaveTo(&SaveWidget)
        FinishWidget(SaveWidget);
        SaveWidget->Save = std::move(Save);
        SaveWidget->Owner = this;
        SaveWidget->Reload();

        this->SavesRegion->AddChild(SaveWidget);

        continue;
    }

    this->FetchedSaves.Empty();

    return;
}

void Jafg::WHostSessionScreen_Old::RefetchSavesImpl()
{
    this->FetchedSaves.Reset(this->FetchedSaves.GetSize());

    TArray<LString> SavesPaths { Finder::GetSavesDir() };
    SavesPaths.Append(GetDefault<JUserPreferences>()->AdditionalSavesSearchPaths);

    TArray<LString> Candidats;

    for (LString& SavePath: SavesPaths)
    {
        if (const LPath AsPath = std::move(SavePath); Paths::DoesDirExist(AsPath))
        {
            for (TArray<LString> New = Finder::FindFilesRecursivelyByName(AsPath, "sqlite3.db"); LString& X : New)
            {
                if (Candidats.Contains(X))
                {
                    LOG_WARNING(LogStorage, "Found duplicate [{}].", X)
                    continue;
                }

                Candidats.Emplace(std::move(X));
                continue;
            }
        }

        continue;
    }

    for (LString& Candidat : Candidats)
    {
        LPath AsPath = std::move(Candidat);
        AsPath.PopSubPath();

        TOptional<LString> DisplayName = Saves::GetDisplayName(AsPath / "sqlite3.db");
        if (DisplayName.IsValid() == false)
        {
            LOG_ERROR(LogStorage, "Found corrupt save at [{}].", AsPath)
            continue;
        }

        LTexture2 Preview;
        Preview.LoadFromDisk("/home/mzoesch/EDev/S-Jafg/Content/Textures/Voxels/Grass_Flora.png");
        //Preview.LoadFromDisk(AsPath / "Thumbnail.png");

        this->FetchedSaves.Emplace(std::move(AsPath), false, std::move(*DisplayName), std::move(Preview));

        continue;
    }

    for (const LFetchedSave& Fetched: this->FetchedSaves)
    {
        LOG_WARNING(LogTemporal, "[{}] {} => {}", Fetched.IsPreviewTextureValid(), Fetched.DisplayName, Fetched.Path)
    }

    return;
}
