// Copyright mzoesch. All rights reserved.

#include "Widgets/Importer.h"
#include "Engine/Engine.h"
#include "Framework/AssetDiscoverer.h"
#include "Nodes/Button.h"
#include "Nodes/Node.h"
#include "Nodes/UserWidget.h"
#include "Nodes/DismissibleFloatingWidget.h"
#include "Nodes/EditableTextButton.h"
#include "Nodes/HParent.h"
#include "Nodes/Spacer.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "Nodes/VParent.h"
#include "User/UserPreferences.h"

void Jafg::CreateImporter(LViewport& Viewport, finder::path const& Path, Detail::EAsset Asset)
{
    LOG_VERBOSE(LogWidgets, "[{}]: Called for  [{}].", Path, LexToString(Asset))

    constexpr LNodeSize2 FloatingSize{480_spt, 260};
    ConstructDeferredWidget(TNodeStaticInit<WDismissibleFloatingWidget>{.Outer=Viewport}).Style()
        .CreateResizeUi(false)
        .InitialWindowPosition({
            static_cast<f32>(Viewport.GetExtent().width)/2.0f - FloatingSize.Size.x/2.0f,
            static_cast<f32>(Viewport.GetExtent().height)/2.0f - FloatingSize.Size.y/2.0f,
            })
        .InitialTitle(algo::sprintf("Import \"{}\"", Path.filename().string()))
        .InitialWindowSize(FloatingSize)
        .Content([Path, Asset](WFloatingWidget& FloatingWidget, WParent& Container)
        {
            BeginStyling(Container).StaticRoot<Detail::WImporter>(Path, Asset);
        });

    return;
}

void Jafg::Detail::WImporter::Construct()
{
    Super::Construct();

    constexpr LNodeSize2 MinTextSize{128_spt, 0};
    constexpr LNodeSize2 InputSize{256_spt, 0};
    constexpr LNodeSize2 ExtensionSpace{64_spt, 0};
    constexpr LNodeSize1 TextInputSpce{16_spt};

    auto& Prefs{GetSingleton<JUserPreferences>()};

    check(this->Path.native().starts_with(LITERAL_TEXT("Content/")))
    this->BasePath = finder::relative(this->Path, LITERAL_TEXT("Content/"));

    BeginStyling(*this).StaticRoot<WVParent>()
        .Anchor(Anchors::VFill+Anchors::HCenter)
        .MaxDesiredSize({480_spt, 0})
        .Space(4_spt)
        .Padding({16_spt, 8})
    [
        NewStaticNode(WHParent)
            .Anchor(EAnchor::HFill)
            .Space(TextInputSpce)
        [
            NewStaticNode(WText)
                .Anchor(EAnchor::HFill)
                .MinDesiredSize(MinTextSize)
                .Content("Texture:")
            +  NewStaticNode(WEditableTextButton)
                .Palette(*Prefs.InputPaletteSolid)
                .MinMaxDesiredSize(InputSize)
                .Enabled(false)
                .Content(this->BasePath.string())
        ]
        + NewStaticNode(WHParent)
            .Anchor(EAnchor::HFill)
            .Space(TextInputSpce)
        [
            NewStaticNode(WText)
                .Anchor(EAnchor::HFill)
                .MinDesiredSize(MinTextSize)
                .Content("Target:")
            + NewStaticNode(WHParent)
            [
                NewStaticNode(WEditableTextButton)
                    .Palette(*Prefs.InputPaletteSolid)
                    .MinMaxDesiredSize(ExtensionSpace)
                    .Content("Content/")
                    .Enabled(false)
                + NewStaticNode(WEditableTextButton).SaveTo(&this->Target)
                    .Palette(*Prefs.InputPaletteSolid)
                    .MinMaxDesiredSize(InputSize - ExtensionSpace*2)
                    .Content(finder::path{BasePath}.replace_extension().string())
                +  NewStaticNode(WEditableTextButton)
                    .Palette(*Prefs.InputPaletteSolid)
                    .MinMaxDesiredSize(ExtensionSpace)
                    .Content(".jasset")
                    .Enabled(false)
            ]
        ]
        + NewStaticNode(WHParent)
            .Space(TextInputSpce)
        [
            NewStaticNode(WText)
                .Anchor(EAnchor::HFill)
                .MinDesiredSize(MinTextSize)
                .Content("Format:")
            +  NewStaticNode(WEditableTextButton).SaveTo(&this->Format)
                .Palette(*Prefs.InputPaletteSolid)
                .MinMaxDesiredSize(InputSize)
                .Content("eR8G8B8A8Srgb")
        ]
        + NewStaticNode(WHParent)
            .Space(TextInputSpce)
        [
            NewStaticNode(WText)
                .Anchor(EAnchor::HFill)
                .MinDesiredSize(MinTextSize)
                .Content("Mip Levels:")
            +  NewStaticNode(WEditableTextButton).SaveTo(&this->MipLevels)
                .Palette(*Prefs.InputPaletteSolid)
                .MinMaxDesiredSize(InputSize)
                .Content("1")
        ]
        + NewStaticNode(WHParent)
            .Space(TextInputSpce)
        [
            NewStaticNode(WText)
                .Anchor(EAnchor::HFill)
                .MinDesiredSize(MinTextSize)
                .Content("Samples:")
            +  NewStaticNode(WEditableTextButton).SaveTo(&this->MaxSampleCount)
                .Palette(*Prefs.InputPaletteSolid)
                .MinMaxDesiredSize(InputSize)
                .Content("e1")
        ]
        + NewStaticNode(WSpacer).Anchor(EAnchor::VFill)
        + NewStaticNode(WText).SaveTo(&this->ErrorMessage)
            .Visibility(ENodeVisibility::Hidden)
            .TextTint(Colors::Red)
        + NewStaticNode(WHParent).Space(8_spt)
            .Anchor(EAnchor::HRight)
        [
            NewStaticNode(WTextButtonIconizedDouble)
                .Palette(*Prefs.PrimaryPaletteSolid)
                .InAllTextBrushes<&LTextBrush::TextScale>(ETextScale::SubHeader)
                .Content("Cancel")
                .OnKeyEventFocused([](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                        && Self.AabbTest({.Translation=Info.Translation}, Info.CursorLocation))
                    {
                        if (auto* Outer{Self.GetMostOuterParent().As<WFloatingWidget>()})
                        {
                            Outer->DestroyFloatingWidgetControlled();
                        }
                        return LNodeReply::Handled();
                    }
                    return LNodeReply::Unhandled();
                })
            + NewStaticNode(WTextButtonIconizedDouble)
                .Palette(*Prefs.PrimaryPaletteSolid)
                .InAllTextBrushes<&LTextBrush::TextScale>(ETextScale::SubHeader)
                .Content("Import")
                .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                        && Self.AabbTest({.Translation=Info.Translation}, Info.CursorLocation))
                    {
                        auto& Assets{*Self.GetMutableEngine().GetSubsystemChecked<JAssetDiscoverer>()};
                        auto Target{finder::content_dir()}; Target/=this->Target->GetContent(); Target.concat(".jasset");
                        Target = weakly_canonical(Target);

                        if (!finder::descendant_of(Target, finder::content_dir()))
                        {
                            check(this->ErrorMessage)
                            LOG_ERROR(LogWidgets, "[{}]: Target must be located in the Content directory.", Target)
                            this->ErrorMessage->SetVisibility(ENodeVisibility::TransitiveHitTestInvisible);
                            this->ErrorMessage->SetContent("Target must be located in the Content directory.");
                            return LNodeReply::Handled();
                        }

                        if (finder::exists(Target))
                        {
                            check(this->ErrorMessage)
                            LOG_ERROR(LogWidgets, "[{}]: Target already exists. Please choose a different target or delete the existing file.", Target)
                            this->ErrorMessage->SetVisibility(ENodeVisibility::TransitiveHitTestInvisible);
                            this->ErrorMessage->SetContent("Already exists. Choose a different target or delete the existing file.");
                            return LNodeReply::Handled();
                        }

                        Assets.PushAsset(Target, {
                            .Texture = this->Path,
                            .Format = serde::from_string<vk::Format>(this->Format->GetContent()),
                            .MipLevels = this->MipLevels->GetContent().empty() ? std::optional<u32>{} : serde::from_string<u32>(this->MipLevels->GetContent()),
                            .MaxSampleCount = serde::from_string<vk::SampleCountFlagBits>(this->MaxSampleCount->GetContent()),
                            });

                        if (auto* Outer{Self.GetMostOuterParent().As<WFloatingWidget>()})
                        {
                            Outer->DestroyFloatingWidgetControlled();
                        }
                        return LNodeReply::Handled();
                    }
                    return LNodeReply::Unhandled();
                })
        ]
    ];

    return;
}
