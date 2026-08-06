// Copyright mzoesch. All rights reserved.

#include "Widgets/EditorFinder.h"
#include "Engine/Engine.h"
#include "Framework/AssetDiscoverer.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/VParent.h"
#include "Nodes/TextBox.h"
#include "Nodes/GenericTabInfos.h"
#include "Nodes/HButton.h"
#include "Nodes/HParent.h"
#include "Nodes/TextButton.h"
#include "Nodes/Text.h"
#include "Nodes/TextIconized.h"
#include "Nodes/VRegion.h"
#include "Platform/SurfaceGlfw3.h"
#include "User/UserPreferences.h"
#include "Widgets/Importer.h"
#include "Nodes/DropDown.h"
#include "Nodes/EditableTextButton.h"
#include "Widgets/AssetInspectors.h"
#include "Framework/Editor.h"

void Jafg::WFinder::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JUserPreferences>()};

    BeginStyling(*this).StaticRoot<WVRegion>()
        .Anchor(EAnchor::Fill)
        .Tint(*Prefs.ForegroundColor)
    [
        NewStaticNode(WHRegion)
            .Anchor(EAnchor::HFill)
            .Padding({4_spt})
            .Tint(*Prefs.AccentColor)
            .Space(4_spt)
        [
            NewStaticNode(WTextButtonIconizedDouble)
                .Anchor(EAnchor::VCenter)
                .InAllLeftIconBrushesChained<&LIconBrush::InwardsPadding, &LIconBrush::MinIconSize, &LIconBrush::Alignment>
                    (0_spt, 14_spt, LIconBrush::Align::Center)
                .InAllRightIconBrushesChained<&LIconBrush::InwardsPadding, &LIconBrush::MinIconSize, &LIconBrush::Alignment>
                    (0_spt, 10_spt, LIconBrush::Align::Center)
                .LeftIcon("Icons/Jafg.Filter")
                .RightIcon("Icons/Jafg.ExtendDown")
                .Style(Prefs.EditorSecondaryButton<LBoxBrush>())
                .InBrush<EStyleBits::Normal, &LBoxBrush::bSkipBrushDraw>(true)
                .InAllBrushesChained<&LBoxBrush::Radii, &LBoxBrush::Padding>(LVec4F{5.0f}, {5_spt, 0.0f})
                .InAllBrushes<&LBoxBrush::Radii>(maths::zero_vector<LVec4F>)
                .OnKeyEventFocused([](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                        && Self.AabbTest({.Translation=Info.Translation}, Info.CursorLocation))
                    {
                    }
                    return LNodeReply::Unhandled();
                })
            + NewStaticNode(WTextBox)
                .Anchor(EAnchor::HFill)
                .Tint(*Prefs.InputColor)
                .OutlineTint(Colors::Gray)
                .OutlineThickness(1)
                .Content("Content")
            + NewStaticNode(WTextButtonIconizedDouble)
                .Anchor(EAnchor::VCenter)
                .Content("Refresh")
                .Style(Prefs.EditorSecondaryButton<LBoxBrush>())
                .InAllBrushes<&LBoxBrush::Radii>(maths::zero_vector<LVec4F>)
                .LeftIcon("Icons/Jafg.Refresh")
                .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                        && Self.AabbTest({.Translation=Info.Translation}, Info.CursorLocation))
                    {
                        this->Repopulate();
                    }
                    return LNodeReply::Unhandled();
                })
        ]
        + NewStaticNode(WHRegion)
            .Anchor(EAnchor::HFill)
            .Tint({0x80})
            .Padding({1_spt})
            .Space(1_spt)
        [
            NewStaticNode(WTextButton)
                .Anchor(EAnchor::HFill)
                .MinDesiredSize(this->NameSize)
                .Content("Name")
                .Style(Prefs.EditorSecondaryButton<LBoxBrush>())
                .InAllBrushes<&LBoxBrush::Radii>(maths::zero_vector<LVec4F>)
            + NewStaticNode(WTextButton)
                .MinMaxDesiredSize(this->SizeSize)
                .Content("Size")
                .Style(Prefs.EditorSecondaryButton<LBoxBrush>())
                .InAllBrushes<&LBoxBrush::Radii>(maths::zero_vector<LVec4F>)
            + NewStaticNode(WTextButton)
                .MinMaxDesiredSize(this->ModifiedSize)
                .Content("Modified")
                .Style(Prefs.EditorSecondaryButton<LBoxBrush>())
                .InAllBrushes<&LBoxBrush::Radii>(maths::zero_vector<LVec4F>)
        ]
        + NewStaticNode(WScrollRegion)
            .SkipBrushDraw(true)
        [
            NewStaticNode(WVParent).SaveTo(&this->Container).Anchor(EAnchor::Fill)
        ]
    ];

    this->Repopulate();

    return;
}

void Jafg::WFinder::ExpandDirty(finder::path const& Path)
{
    LOG_TRACE(LogWidgets, "[{}]: Expanding path.", Path.string())

    finder::recursive_directory_iterator It{Path};
    for (auto& Entry: It)
    {
        if (Entry.is_directory())
        {
            this->Meta[Entry.path()].bExtended = true;
        }
    }

    return;
}

void Jafg::WFinder::CollapseDirty(finder::path const& Path)
{
    LOG_TRACE(LogWidgets, "[{}]: Collapsing path.", Path.string())

    finder::recursive_directory_iterator It{Path};
    this->Meta[Path].bExtended = false;
    for (auto& Entry: It)
    {
        if (Entry.is_directory())
        {
            this->Meta[Entry.path()].bExtended = false;
        }
    }

    return;
}

void Jafg::WFinder::Repopulate()
{
    check(this->Container)
    this->Container->RemoveChildren();
    auto Counter{0uz};
    auto Where{0uz};
    this->Populate(Counter, Where, 0uz, finder::path{LITERAL_TEXT("Content")});
    this->Repaint();

    return;
}

void Jafg::WFinder::Populate(std::size_t& Counter, std::size_t& Where, std::size_t Indent, finder::path const& Path)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    finder::directory_iterator It{Path};
    TArray<finder::directory_entry> Entries;
    algo::for_each(It, [&Entries](auto const& Entry){ Entries.emplace_back(Entry); });

    if (*Prefs.EditorSortDirectoriesFirst)
    {
        algo::sort(Entries, [](auto const& A, auto const& B)
        {
            if (A.is_directory() && !B.is_directory())
            {
                return true;
            }
            if (!A.is_directory() && B.is_directory())
            {
                return false;
            }
            return algo::lexicographical_string_compare(A.path().filename().string(), B.path().filename().string());
        });
    }
    else
    {
        algo::sort(Entries, algo::lexicographical_string_compare, [](auto const& Entry){ return Entry.path().filename().string(); });
    }

    for (auto& Entry: Entries)
    {
        this->Populate(Counter, Where, Indent, Entry);
    }

    return;
}

void Jafg::WFinder::Populate(std::size_t& Counter, std::size_t& Where, std::size_t Indent, finder::directory_entry const& Entry)
{
    check(this->Container)

    if (Entry.is_directory())
    {
        auto& EntryMeta{this->Meta[Entry.path()]};
        EntryMeta.Node = &this->Container->AddChildAt(Where++, NewStaticNode(WHButton)
            .Visibility(ENodeVisibility::Visible)
            .Anchor(EAnchor::HFill)
            .Padding(this->Indent2Padding(Indent))
            .Style(GetSingleton<JUserPreferences>().EditorProximityBoxStyle2<LRegionBrush>(Counter++))
            .OnKeyEventFocused([this, SelfPath=Entry.path()](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                    {
                        auto& SelfMeta{this->Meta[SelfPath]};
                        SelfMeta.bExtended = !SelfMeta.bExtended;
                        if (Event.Mods & EModBits::Alt)
                        {
                            if (SelfMeta.bExtended)
                            {
                                this->ExpandDirty(SelfPath);
                            }
                            else
                            {
                                this->CollapseDirty(SelfPath);
                            }
                        }

                        this->OnExtendUpdate(SelfPath, SelfMeta);
                    }
                    return LNodeReply::Unhandled();
                })
        [
            NewStaticNode(WTextIconizedLeft)
                .Anchor(EAnchor::HFill)
                .MinDesiredSize(this->NameSize)
                .Icon(EntryMeta.bExtended ? LString{"Icons/Jafg.ExtendDown"} : LString{"Icons/Jafg.ExtendRight"})
                .Content(Entry.path().filename().string())
            + NewStaticNode(WText)
                .MinMaxDesiredSize(this->SizeSize)
                .Content(finder::human_readable_entry_size(Entry))
            + NewStaticNode(WText)
                .MinMaxDesiredSize(this->ModifiedSize)
                .Content(finder::human_readable_time(Entry.last_write_time()))
        ].Unique());

        if (EntryMeta.bExtended)
        {
            this->Populate(Counter, Where, Indent + 1, Entry.path());
        }
    }
    else
    {
        this->Container->AddChildAt(Where++, NewStaticNode(WHButton)
            .Anchor(EAnchor::HFill)
            .Padding(this->Indent2Padding(Indent))
            .Style(GetSingleton<JUserPreferences>().EditorProximityBoxStyle2<LRegionBrush>(Counter++))
            .OnKeyEventFocused([this, SelfPath=Entry.path()](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Info.CursorLocation && Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
                {
                    if (SelfPath.has_extension() && SelfPath.extension() == Detail::AssetPostfix)
                    {
                        auto& Asset{GetEngine().GetSubsystemChecked<JAssetDiscoverer>()->GetHeader(SelfPath)};

                        CreateDropDownMenu(this->GetViewport(), *Info.CursorLocation, {}, {
                            LDropDownNodeSeparator{.DisplayName = "INSPECT"},
                            LDropDownNodeOption{
                                .Selector = {
                                    .DisplayName = algo::sprintf("Inspect {}", LexToString(Asset.Type)),
                                    .Icon = "Icons/Jafg.Information"
                                    },
                                .OnAction = [SelfPath](auto&&...)
                                {
                                    check(GetSingleton<JUserPreferences>().Editor)
                                    CreateTextureInspector(*GetSingleton<JUserPreferences>().Editor, SelfPath);
                                    return algo::reply::unhandled();
                                },},
                            });
                    }
                    else
                    {
                        CreateDropDownMenu(this->GetViewport(), *Info.CursorLocation, {}, {
                            LDropDownNodeSeparator{.DisplayName = "IMPORT"},
                            LDropDownNodeOption{
                                .Selector = {
                                    .DisplayName = "Import as Texture",
                                    .Icon = "Icons/Jafg.Import"
                                    },
                                .OnAction = [this, SelfPath](auto&&...)
                                {
                                    CreateImporter(this->GetViewport(), SelfPath, Detail::EAsset::Texture);
                                    return algo::reply::unhandled();
                                },},
                            });
                    }
                    return LNodeReply::Handled();
                }
                return LNodeReply::Unhandled();
            })
        [
            NewStaticNode(WTextBoxIconizedLeft) // TODO: replace with WTextIconizedLeft (but
                .Anchor(EAnchor::HFill)
                .Padding({})
                .MinDesiredSize(this->NameSize)
                .SkipBrushDraw(true)
                .Icon("Icons/Jafg.File")
                .Content(Entry.path().filename().string())
            + NewStaticNode(WText)
                .MinMaxDesiredSize(this->SizeSize)
                .Content(finder::human_readable_entry_size(Entry))
            + NewStaticNode(WText)
                .MinMaxDesiredSize(this->ModifiedSize)
                .Content(finder::human_readable_time(Entry.last_write_time()))
        ].Unique());
    }

    return;
}

void Jafg::WFinder::OnExtendUpdate(finder::path const& Path, LEntry& Entry)
{
    check(this->Container)

    check(Entry.Node.IsValidFast())
    auto& Parent{Entry.Node->AsStatic<WParent>()};
    Parent.GetChildren()[0]->AsStatic<WTextIconizedLeft>().Icon =
        Entry.bExtended ? LTexture2::FromAsset("Icons/Jafg.ExtendDown") : LTexture2::FromAsset("Icons/Jafg.ExtendRight");

    auto Idx{algo::distance(this->Container->GetChildren(), algo::find_checked(this->Container->GetChildren(), &Parent, algo::unique_raw))};
    auto Indent{static_cast<std::size_t>(this->Padding2Indent(Parent.Padding))};

    check(this->Container == Parent.GetParentChecked())
    if (Entry.bExtended)
    {
        auto Where{Idx + 1uz};
        auto Dummy{0uz};
        this->Populate(Dummy, Where, Indent + 1, Path);
    }
    else
    {
        auto Where{Idx + 1uz};
        auto& Children{this->Container->GetChildren()};
        while (algo::valid_index(Children, Where) && static_cast<std::size_t>(this->Padding2Indent(Children[Where]->AsStatic<WParent>().Padding)) > Indent)
        {
            Children[Where]->RemoveFromParent2();
        }
        JAFG_PLATFORM_NO_DISCARD_CTRL_PATH
    }

    this->Repaint();

    return;
}

void Jafg::WFinder::Repaint()
{
    check(this->Container)

    auto& Surface{this->GetViewport().GetSurface()};
    bool bMouse{Surface.HasMouseLocationForOrtho()};
    auto Loc{Surface.GetMouseLocation()};

    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto Counter{0uz};
    for (auto& Child: this->Container->GetChildren())
    {
        auto& Button{Child->AsStatic<WHButton>()};
        Button.Style = Prefs.EditorProximityBoxStyle2<LRegionBrush>(Counter++);

        if (!Button.IsEnabled())
        {
            Button._ButtonBase_SetBrush(EStyleBits::Disabled);
        }
        else if (Button.IsSelected())
        {
            Button._ButtonBase_SetBrush(EStyleBits::Selected);
        }
        else if (bMouse)
        {
            // TODO: translation
            if (Button.AabbTest({.Translation=maths::zero_vector<LVec2F>}, *Loc))
            {
                Button._ButtonBase_SetBrush(EStyleBits::Hover);
            }
            else
            {
                Button._ButtonBase_SetBrush(EStyleBits::Normal);
            }
        }
    }

    return;
}
