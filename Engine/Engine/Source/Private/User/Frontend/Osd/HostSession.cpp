// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/HostSession.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Storage/SaveFunctions.h"
#include "System/EnginePath.h"
#include "User/UserPreferences.h"
#include "Widgets/Button.h"
#include "Widgets/EditableTextBox.h"
#include "Widgets/Switcher.h"
#include "Widgets/Region.h"
#include "Widgets/ScrollRegion.h"
#include "Widgets/VRegion.h"
#include "Widgets/HRegion.h"
#include "Widgets/Spacer.h"
#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "System/TextureSubsystem.h"

#define BACKGROUND_COLOR_HOST {40, 39, 49}
#define DEFAULT_SESSION_NAME "My Session"

namespace
{

namespace EBuildReason
{

enum Type
{
    Default,
    Edit,
};

} /* ~Namespace EBuildReason */

Jafg::TWidgetFactoryVRegion<Jafg::WVRegion>* _BuildGeneral
(
    Jafg::JCxxClass*         Context,
    LString&&                Header,
    const EBuildReason::Type Reason,
    Jafg::WEditableTextBox** SessionName = nullptr,
    Jafg::WTextBox**         SessionPath = nullptr
)
{
    using namespace Jafg;

    return &NewNodeCtx(Context, WVRegion)
        .Anchor(EAnchor::Fill)
        .VSpace(5)
    [
        NewNodeCtx(Context, WTextBox)
            .Anchor(EAnchor::HCenter)
            .Content(std::move(Header))
            .Brush(LTextBoxBrush::Header())
        +
        NewNodeCtx(Context, WTextBox)
            .Anchor(EAnchor::HLeft)
            .Content("Name")
            .Brush(LTextBoxBrush::Header())
        +
        NewNodeCtx(Context, WEditableTextBox).SaveTo(SessionName)
            .Anchor(EAnchor::HFill)
            .Type(ERegionBrush::OutlineBox)
            .OutlineThickness(1)
            .OutlineTint(Lal::LColor::DarkGray)
            .TextScale(LTextBoxBrush::SubHeader().TextScale)
            .PlaceholderContent(DEFAULT_SESSION_NAME)
        +
        NewNodeCtx(Context, WTextBox).SaveTo(SessionPath)
            .Anchor(EAnchor::HLeft)
            .Content("")
            .Brush(LTextBoxBrush::SubHeader())
            .TextColor(Lal::LColor::Gray)
        +
        NewNodeCtx(Context, WSpacer).Height(20_pt)
        +
        NewNodeCtx(Context, WHRegion)
            .Anchor(EAnchor::HFill)
        [
            NewNodeCtx(Context, WTextBox)
                .Content("Templates")
                .Brush(LTextBoxBrush::Body())
        ]
        +
        NewNodeCtx(Context, WSpacer).Height(20_pt)
        +
        NewNodeCtx(Context, WTextBox)
            .Anchor(EAnchor::HLeft)
            .Content("Player")
            .Brush(LTextBoxBrush::SubHeader())
        +
        NewNodeCtx(Context, WHRegion)
            .Anchor(EAnchor::HFill)
        [
            NewNodeCtx(Context, WTextBox)
                .Content("Perma Death")
                .Brush(LTextBoxBrush::Body())
        ]
        +
        NewNodeCtx(Context, WSpacer).Height(20_pt)
        +
        NewNodeCtx(Context, WTextBox)
            .Anchor(EAnchor::HLeft)
            .Content("Enemy")
            .Brush(LTextBoxBrush::SubHeader())
        +
        NewNodeCtx(Context, WHRegion)
            .Anchor(EAnchor::HFill)
        [
            NewNodeCtx(Context, WTextBox)
                .Content("No enemies")
                .Brush(LTextBoxBrush::Body())
        ]
        +
        NewNodeCtx(Context, WHRegion)
            .Anchor(EAnchor::HFill)
        [
            NewNodeCtx(Context, WTextBox)
                .Content("Max Temperament")
                .Brush(LTextBoxBrush::Body())
            +
            NewNodeCtx(Context, WTextBox)
                .Content("Passive")
                .Brush(LTextBoxBrush::Body())
            +
            NewNodeCtx(Context, WTextBox)
                .Content("Docile")
                .Brush(LTextBoxBrush::Body())
            +
            NewNodeCtx(Context, WTextBox)
                .Content("Default")
                .Brush(LTextBoxBrush::Body())
        ]
    ]
    ;
}

Jafg::TWidgetFactoryVRegion<Jafg::WVRegion>* _BuildMultiplayer
(
    Jafg::JCxxClass*         Context,
    LString&&                Header,
    const EBuildReason::Type Reason,
    Jafg::WTextBox**         OutHeader = nullptr
)
{
    using namespace Jafg;

    return &NewNodeCtx(Context, WVRegion)
        .Anchor(EAnchor::Fill)
        .VSpace(5)
    [
        NewNodeCtx(Context, WTextBox).SaveTo(OutHeader)
            .Anchor(EAnchor::HCenter)
            .Content(std::move(Header))
            .Brush(LTextBoxBrush::Header())
        +
        NewNodeCtx(Context, WHRegion)
            .Anchor(EAnchor::HFill)
        [
            NewNodeCtx(Context, WTextBox)
                .Content("Multiplayer")
                .Brush(LTextBoxBrush::Body())
        ]
        +
        NewNodeCtx(Context, WHRegion)
            .Anchor(EAnchor::HFill)
        [
            NewNodeCtx(Context, WTextBox)
                .Content("Open To Lan")
                .Brush(LTextBoxBrush::Body())
        ]
        +
        NewNodeCtx(Context, WHRegion)
            .Anchor(EAnchor::HFill)
        [
            NewNodeCtx(Context, WTextBox)
                .Content("Sync with Steam")
                .Brush(LTextBoxBrush::Body())
        ]
        +
        NewNodeCtx(Context, WHRegion)
            .Anchor(EAnchor::HFill)
        [
            NewNodeCtx(Context, WTextBox)
                .Content("Friends Only")
                .Brush(LTextBoxBrush::Body())
        ]
        +
        NewNodeCtx(Context, WHRegion)
            .Anchor(EAnchor::HFill)
        [
            NewNodeCtx(Context, WTextBox)
                .Content("Invite Only")
                .Brush(LTextBoxBrush::Body())
        ]
    ]
    ;
}

} /* ~Namespace <Anonymous> */

void Jafg::WHostSessionScreen::Construct()
{
    Super::Construct();

    Private::JHostSessionScreenData* Data{ NewObject<Private::JHostSessionScreenData>(this->GetOuter()) };
    Data->Screen = this;

    MakeRootNode(WSwitcher).SaveTo(&this->Switcher)
        .Anchor(EAnchor::Fill)
    [
        NewNode(WHostSessionScreen_Old).SaveTo(&this->OldScreen)
            .Data(*Data)
        +
        NewNode(WHostSessionScreen_New).SaveTo(&this->NewScreen)
            .Data(*Data)
        +
        NewNode(WHostSessionScreen_Old_Host).SaveTo(&this->OldScreenHost)
            .Data(*Data)
    ]
    FinishWidgetStyling()

    this->Switcher->SetActiveWidget(this->OldScreen);

    Data->MarkAsGarbage_v2();

    return;
}

void Jafg::WHostSessionScreen::OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility)
{
    Super::OnVisibilityChanged(InOldVisibility, InNewVisibility);

    if (EWidgetVisibility::IsDrawn(InNewVisibility))
    {
        if (this->OldScreen && this->OldScreenHost && (this->OldScreenHost->IsCachedSaveValid() == false))
        {
            this->OldScreen->HighlightNoSave(true);
        }

        if (this->Switcher)
        {
            if (this->Switcher->GetActiveNode() == this->OldScreenHost)
            {
                this->OldScreenHost->Reset();
                this->Switcher->SetActiveWidget(this->OldScreen);
            }
        }
    }

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

void Jafg::WHostSessionScreen::ShowOldScreenHost()
{
    check( this->Switcher )
    check( this->OldScreenHost )

    this->Switcher->SetActiveWidget(this->OldScreenHost);

    return;
}

Jafg::WHostSessionScreen_New::WHostSessionScreen_New(LCxxObjectInitializer const& ObjectInitializer)
    : Super(ObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    return;
}

void Jafg::WHostSessionScreen_New::Construct()
{
    this->SetType(ERegionBrush::Box);
    this->SetTint(BACKGROUND_COLOR_HOST);

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

bool Jafg::WHostSessionScreen_New::AddData(JNodeData& Data)
{
    const bool bSuper{ Super::AddData(Data) };
    Private::JHostSessionScreenData* HssData{ Data.As<Private::JHostSessionScreenData>() };
    if (HssData == nullptr)
    {
        return bSuper;
    }

    this->Owner = HssData->Screen;
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

Jafg::TWidgetFactoryHRegion<Jafg::WHRegion>* Jafg::WHostSessionScreen_New::AddMenuButtons()
{
    return &NewNode(WHRegion)
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
                LOG_VERBOSE(LogWidgets, "Trying to host from new save.")

                if (this->GetOuter()->IsWorld() == false)
                {
                    LOG_ERROR(LogWidgets, "Cannot host, because outer is not a world.")
                    return;
                }

                LString Name { DEFAULT_SESSION_NAME };
                if (this->SessionName && this->SessionName->GetContent().empty() == false)
                {
                    Name = this->SessionName->GetContent();
                }

                const Saves::LMinimalMetaData Meta
                {
                    Name
                };

                LString Error;
                const LPath Path = Finder::GetSavesDir() / Name;
                if (Saves::CreateNewSave(Path, Meta, &Error) == false)
                {
                    LOG_ERROR(LogWidgets, "Failed to create new save entry. Reason: [{}].", Error)
                    return;
                }

                this->GetEngine().Browse
                (
                    static_cast<LWorld*>(this->GetOuter()),
                    Lal::SprintF("{}?Save={}", Name_LevelMyWorld.ToString(), Path)
                );

                return;
            })
    ]
    ;
}

void Jafg::WHostSessionScreen_New::OnLoad_General(WTabBar* TabBar, WNode* Button, WNode* Panel)
{
    WVRegion* Region = DynamicCast<WVRegion>(Panel);
    jassert( Region )

    check( Region->GetChildren().empty() )

    Region->SetAnchor(EAnchor::Fill);
    Region->SetPadding({25.0f});

    WTextBox* SessionPath;

    Region->GetFactory<WVRegion>()
    [
        NewNode(WScrollRegion)
            .Anchor(EAnchor::Fill)
        [
            _BuildGeneral(this, "Host New Session", EBuildReason::Default, &this->SessionName, &SessionPath)
        ]
        +
        this->AddMenuButtons()
    ]
    ;

    Region->MakeChildrenFinal();

    if (this->SessionName)
    {
        SessionPath->SetContent(Lal::SprintF("Path: {}/{}", Finder::GetSavesDir(), DEFAULT_SESSION_NAME));
        this->SessionName->OnContentChanged.BindStrong([this, SessionPath](const LString& NewContent) -> void
        {
            if (IsValidFast(this->GetOuter(), SessionPath))
            {
                if (NewContent.empty())
                {
                    SessionPath->SetContent(Lal::SprintF("Path: {}/{}", Finder::GetSavesDir(), DEFAULT_SESSION_NAME));
                }
                else
                {
                    SessionPath->SetContent(Lal::SprintF("Path: {}/{}", Finder::GetSavesDir(), NewContent));
                }
            }

            return;
        });
    }

    return;
}

void Jafg::WHostSessionScreen_New::OnLoad_Multiplayer(WTabBar* TabBar, WNode* Button, WNode* Panel)
{
    WVRegion* Region = DynamicCast<WVRegion>(Panel);
    jassert( Region )

    check( Region->GetChildren().empty() )

    Region->SetAnchor(EAnchor::Fill);
    Region->SetPadding(25.0f);

    Region->GetFactory<WVRegion>()
    [
        NewNode(WScrollRegion)
            .Anchor(EAnchor::Fill)
        [
            _BuildMultiplayer(this, "Host New Session", EBuildReason::Default)
        ]
        +
        this->AddMenuButtons()
    ]
    ;

    Region->MakeChildrenFinal();

    return;
}

void Jafg::WHostSessionScreen_New::OnLoad_WorldGeneration(WTabBar* TabBar, WNode* Button, WNode* Panel)
{
    WVRegion* Region = DynamicCast<WVRegion>(Panel);
    jassert( Region )

    Region->SetAnchor(EAnchor::Fill);
    Region->SetTint(BACKGROUND_COLOR_HOST);

    return;
}

void Jafg::WHostSessionScreen_New::OnLoad_Policies(WTabBar* TabBar, WNode* Button, WNode* Panel)
{
    WVRegion* Region = DynamicCast<WVRegion>(Panel);
    jassert( Region )

    Region->SetAnchor(EAnchor::Fill);
    Region->SetTint(BACKGROUND_COLOR_HOST);

    return;
}

void Jafg::WHostSessionScreen_New::OnLoad_Advanced(WTabBar* TabBar, WNode* Button, WNode* Panel)
{
    WVRegion* Region = DynamicCast<WVRegion>(Panel);
    jassert( Region )

    Region->SetAnchor(EAnchor::Fill);
    Region->SetTint(BACKGROUND_COLOR_HOST);

    return;
}

Jafg::WHostSessionScreen_Old_Save::WHostSessionScreen_Old_Save(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetAnchor(EAnchor::HFill);
    return;
}

void Jafg::WHostSessionScreen_Old_Save::Reload()
{
    this->OptionalHolder.reset();

    if (this->IsSaveValid() == false)
    {
        LOG_WARNING(LogWidgets, "Could not reload save widget because the stored save is invalid.")
        return;
    }

    this->RemoveChildren();

    if (this->Save->PreviewTexture.IsValid() == false)
    {
        this->OptionalHolder = GetDefault<JTextureSubsystem>()->GetTexture
        ({
            EEnginePaths::Interface, "NoImage.png"
        }, ERawImageFormat::BGR8);
    }

    WRegion* Thumbnail;
    WHRegion* Region;
    NewNode(WHRegion).SaveTo(&Region)
        .Anchor(EAnchor::Fill)
        .Padding(10)
    [
        NewNode(WRegion).SaveTo(&Thumbnail)
            .MinDesiredSize({100_pt, 100})
            .Type(ERegionBrush::OutlineBox)
            .Tint(Lal::LColor::White)
            .OutlineTint(Lal::LColor::Black)
            .ImageBehavior(EImageBehavior::Aspect)
            .ImageOobm(EImageOobm::Discard)
            .ImagePadding(2)
            .Texture
            (
                  this->Save->PreviewTexture.IsValid()
                ? &this->Save->PreviewTexture
                : GetDefault<JTextureSubsystem>()->GetTexture
                    ({
                        EEnginePaths::Interface, "NoImage.png"
                    }, ERawImageFormat::BGR8).get()
            )
        +
        NewNode(WVRegion)
            .Anchor(EAnchor::Fill)
        [
            NewNode(WTextBox)
                .Content(this->Save->DisplayName)
                .Brush(LTextBoxBrush::SubHeader())
            +
            NewNode(WTextBox)
                .Content(this->Save->Description)
                .Brush(LTextBoxBrush::Body())
            +
            NewNode(WSpacer)
                .Anchor(EAnchor::VFill)
            +
            NewNode(WTextBox)
                .Content(this->Save->Path.generic_string())
                .Brush(LTextBoxBrush::Body())
                .TextColor(Lal::LColor::DarkerGray)
        ]
    ];
    this->AddChild(Region);
    FinishWidget(Region);

    return;
}

void Jafg::WHostSessionScreen_Old_Save::OnPrimaryRelease()
{
    Super::OnPrimaryRelease();

    if (this->Owner)
    {
        this->Owner->HighlightSave(*this);
    }
    else
    {
        LOG_ERROR(LogWidgets, "Owner is invalid.")
    }

    return;
}

Jafg::WHostSessionScreen_Old::WHostSessionScreen_Old(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    return;
}

void Jafg::WHostSessionScreen_Old::Construct()
{
    this->SetPadding(25);
    this->SetType(ERegionBrush::Box);
    this->SetTint(BACKGROUND_COLOR_HOST);

    WVRegion* Region;
    NewNode(WVRegion).SaveTo(&Region)
        .Anchor(EAnchor::Fill)
        .MaxDesiredSize({1000_pt, 0})
        .VSpace(10)
    [
        NewNode(WTextBox)
            .Anchor(EAnchor::CenterCenter)
            .Content("Host Session")
            .Brush(LTextBoxBrush::Header())
        +
        NewNode(WScrollRegion).SaveTo(&this->SavesRegionContainer)
            .Anchor(EAnchor::Fill)
            .Type(ERegionBrush::OutlineBox)
            .Tint({64, 63, 75})
            .OutlineTint(Lal::LColor::Black)
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
                .OnPrimaryRelease([this](WButton* Self, const LKeyEvent& InKeyEvent) -> void
                {
                    check( this->Owner )
                    check( this->SelectedSaveIndex != INDEX_NONE )

                    if
                    (
                        const WHostSessionScreen_Old_Save* Save
                        {
                            DynamicCast<WHostSessionScreen_Old_Save>(this->SavesRegion->GetChildren()[this->SelectedSaveIndex]->Content)
                        };
                        Save
                    )
                    {
                        this->Owner->GetOldScreenHost()->SetSave(Save->GetSave());
                        this->Owner->GetOldScreenHost()->UpdateToCachedSave();
                        this->Owner->ShowOldScreenHost();
                    }
                    else
                    {
                        LOG_ERROR(LogWidgets, "Failed to cast child to save widget.")
                    }

                    return;
                })
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

bool Jafg::WHostSessionScreen_Old::AddData(JNodeData& Data)
{
    const bool bSuper{ Super::AddData(Data) };
    Private::JHostSessionScreenData* HssData{ Data.As<Private::JHostSessionScreenData>() };
    if (HssData == nullptr)
    {
        return bSuper;
    }

    this->Owner = HssData->Screen;
    check( this->Owner )

    return true;
}

void Jafg::WHostSessionScreen_Old::RefetchSaves(const bool bResetHighlight /* = true */)
{
    check( this->SavesRegion )
    this->SavesRegion->RemoveChildren();

    if (bResetHighlight)
    {
        this->HighlightNoSave(true);
    }

    this->RefetchSavesImpl();

    if (this->FetchedSaves->empty())
    {
        WTextBox* Text;
        NewNode(WTextBox).SaveTo(&Text)
            .Anchor(EAnchor::Fill)
            .Brush(LTextBoxBrush::SubHeader())
            .TextHAlign(ETextHAlign::Center)
            .TextVAlign(ETextVAlign::Center)
            .Content("No sessions found. Select \"Host From New\" to create a new sessions.")
        FinishWidget(Text);

        this->SavesRegion->AddChild(Text);

        return;
    }

    for (LFetchedSave& Save : this->FetchedSaves)
    {
        WHostSessionScreen_Old_Save* SaveWidget;
        NewNode(WHostSessionScreen_Old_Save).SaveTo(&SaveWidget);

        SaveWidget->Save = std::move(Save);
        SaveWidget->Owner = this;
        this->SavesRegion->AddChild(SaveWidget);

        FinishWidget(SaveWidget);
        SaveWidget->Reload();

        continue;
    }

    algo::orphan(&this->FetchedSaves);

    return;
}

void Jafg::WHostSessionScreen_Old::HighlightSave(WHostSessionScreen_Old_Save& Who)
{
    check( this->SavesRegion )

    if (this->SelectedSaveIndex != INDEX_NONE)
    {
        if
        (
            WHostSessionScreen_Old_Save* Save = DynamicCast<WHostSessionScreen_Old_Save>(this->SavesRegion->GetChildren()[this->SelectedSaveIndex]->Content);
            Save
        )
        {
            Save->SetBrush(Save->GetNormalBrush());
            Save->SetLetUiReactToEvents(true);
        }
        else
        {
            LOG_ERROR(LogWidgets, "Failed to cast child to save widget.")
        }
    }

    if (auto const It{algo::find(this->SavesRegion->GetChildren(), static_cast<WNode const*>(&Who), [](auto const& E){ return E->Content; }) }; It != this->SavesRegion->GetChildren().end())
    {
        this->SelectedSaveIndex = algo::distance(this->SavesRegion->GetChildren(), It);
        if (this->DeleteButton)
        {
            this->DeleteButton->SetEnabled(true);
        }
        if (this->EditButton)
        {
            this->EditButton->SetEnabled(true);
        }
        if (this->HostButton)
        {
            this->HostButton->SetEnabled(true);
        }

        Who.SetBrush(Who.GetPressBrush());
        Who.SetLetUiReactToEvents(false);
    }
    else
    {
        this->SelectedSaveIndex = INDEX_NONE;
        LOG_ERROR(LogWidgets, "Failed to find fetched save [{}].", Who.GetSave().Path)
    }

    return;
}

void Jafg::WHostSessionScreen_Old::HighlightNoSave(const bool bScrollUp /* = false */)
{
    if (this->SelectedSaveIndex != INDEX_NONE)
    {
        if
        (
            WHostSessionScreen_Old_Save* Save = DynamicCast<WHostSessionScreen_Old_Save>(this->SavesRegion->GetChildren()[this->SelectedSaveIndex]->Content);
            Save
        )
        {
            Save->SetBrush(Save->GetNormalBrush());
            Save->SetLetUiReactToEvents(true);
        }
        else
        {
            LOG_ERROR(LogWidgets, "Failed to cast child to save widget.")
        }
    }

    this->SelectedSaveIndex = INDEX_NONE;

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

    if (bScrollUp)
    {
        if (this->SavesRegionContainer)
        {
            this->SavesRegionContainer->ApplyVScroll(WScrollRegion::MaxScrollUp);
        }
    }

    return;
}

void Jafg::WHostSessionScreen_Old::RefetchSavesImpl()
{
    this->FetchedSaves->clear();

    TArray<LString> SavesPaths { Finder::GetSavesDir() };
    SavesPaths.append_range(GetDefault<JUserPreferences>()->AdditionalSavesSearchPaths);

    TArray<LString> Candidats;

    for (const LString& SavePath: SavesPaths)
    {
        if (const LPath AsPath{ SavePath.begin(), SavePath.end() }; Finder::DoesDirectoryExist(AsPath))
        {
            for (TArray<LString> New = Finder::FindFilesRecursivelyByName(AsPath, "sqlite3.db"); LString& X : New)
            {
                if (algo::contains(Candidats, X))
                {
                    LOG_WARNING(LogStorage, "Found duplicate [{}].", X)
                    continue;
                }

                Candidats.emplace_back(std::move(X));
                continue;
            }
        }

        continue;
    }

    for (LString& Candidat : Candidats)
    {
        LPath AsPath { std::move(Candidat) };
        AsPath = AsPath.parent_path();

        TOptional<LString> DisplayName = Saves::GetDisplayName(AsPath);
        if (DisplayName.has_value() == false)
        {
            LOG_ERROR(LogStorage, "Found corrupt save at [{}].", AsPath)
            continue;
        }

        LTexture2 Preview;
        Preview.LoadFromDisk(AsPath / "Thumbnail.png", ERawImageFormat::BGR8);

        this->FetchedSaves->emplace_back(std::move(AsPath), false, std::move(*DisplayName), "A description of this save.", std::move(Preview));

        continue;
    }

    for (const LFetchedSave& Fetched: this->FetchedSaves)
    {
        LOG_WARNING(LogTemporal, "[{}] {} => {}", Fetched.IsPreviewTextureValid(), Fetched.DisplayName, Fetched.Path)
    }

    return;
}

Jafg::WHostSessionScreen_Old_Host::WHostSessionScreen_Old_Host(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    return;
}

void Jafg::WHostSessionScreen_Old_Host::Construct()
{
    this->SetPadding(25.0f);
    this->SetType(ERegionBrush::Box);
    this->SetTint(BACKGROUND_COLOR_HOST);

    WVRegion* Region;
    NewNode(WVRegion).SaveTo(&Region)
        .Anchor(EAnchor::Fill)
        .VSpace(10)
    [
        NewNode(WScrollRegion)
            .Anchor(EAnchor::Fill)
        [
            _BuildMultiplayer(this, "", EBuildReason::Edit, &this->Header)
        ]
        +
        this->AddMenuButtons()
    ]
    ;

    this->AddChild(Region);

    Super::Construct();

    if (this->IsCachedSaveValid())
    {
        this->UpdateToCachedSave();
    }

    return;
}

bool Jafg::WHostSessionScreen_Old_Host::AddData(JNodeData& Data)
{
    const bool bSuper{ Super::AddData(Data) };
    Private::JHostSessionScreenData* HssData{ Data.As<Private::JHostSessionScreenData>() };
    if (HssData == nullptr)
    {
        return bSuper;
    }

    this->Owner = HssData->Screen;
    check( this->Owner )

    return true;
}

void Jafg::WHostSessionScreen_Old_Host::UpdateToCachedSave()
{
    check( this->IsCachedSaveValid() )

    check( this->Header )
    this->Header->SetContent(Lal::SprintF("Hosting \"{}\"", this->Save->DisplayName));

    return;
}

Jafg::TWidgetFactoryHRegion<Jafg::WHRegion>* Jafg::WHostSessionScreen_Old_Host::AddMenuButtons()
{
    return &NewNode(WHRegion)
        .Anchor(EAnchor::HFill)
        .HSpace(15)
    [
        NewNode(WTextButton)
            .Content("Back")
            .OnPrimaryRelease([this](WButton* Self, const LKeyEvent& InKeyEvent) -> void
            {
                this->Reset();
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
                if (this->IsCachedSaveValid() == false)
                {
                    LOG_ERROR(LogWidgets, "Cannot host, because cached save is not valid.")
                    return;
                }

                if (this->GetOuter()->IsWorld() == false)
                {
                    LOG_ERROR(LogWidgets, "Cannot host, because outer is not a world.")
                    return;
                }

                LOG_VERBOSE(LogWidgets, "Forwarding host request for [{}].", this->Save->Path)
                this->GetEngine().Browse
                (
                    this->GetOuter()->AsWorld(),
                    Lal::SprintF("{}?Save={}", Name_LevelMyWorld.ToString(), this->Save->Path)
                );

                return;
            })
    ]
    ;
}

#undef BACKGROUND_COLOR_HOST
#undef DEFAULT_SESSION_NAME
