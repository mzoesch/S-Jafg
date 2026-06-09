// Copyright mzoesch. All rights reserved.

#include "Widgets/AssetInspectors.h"
#include "Engine/Engine.h"
#include "Framework/AssetDiscoverer.h"
#include "Framework/TextureSubsystem.h"
#include "User/UserPreferences.h"
#include "Widgets/Editor.h"
#include "Nodes/Text.h"
#include "Nodes/VParent.h"
#include "Nodes/VRegion.h"

void Jafg::CreateTextureInspector(LTabOverlayPossibilities& Where, finder::path const& Path)
{
    //# TODO: maybe search first if there is a window inspector of this requested texture?

    LTabCreateInfo Info{
        .Selector = LTabSelectorCreateInfo{
            .DisplayName = Path.filename().replace_extension("").string(),
            .Icon = "Icons/Jafg.Information",
            },
        .Panel = NewNode(Where.Owner.GetViewport()).Class<WTextureInspector>(Path)
        };
    Where.AddWindow(std::move(Info), true);

    return;
}

void Jafg::WTextureInspector::Construct()
{
    Super::Construct();

    check(!this->Asset.empty() && "Path not set.")

    LEngine& Engine{this->GetMutableEngine()};

    auto& Prefs{GetSingleton<JUserPreferences>()};

    auto& Assets{*Engine.GetSubsystemChecked<JAssetDiscoverer>()};
    auto& Textures{*Engine.GetLocalEgo().GetFrontend().GetSubsystemChecked<JTextureSubsystem>()};
    auto& Header{Assets.GetHeader(this->Asset)};

    if (Header.Type != Detail::EAsset::Texture)
    {
        LOG_FATAL(LogWidgets, "[{}]: Invalid asset type. Expected a texture but got [{}]."
            , this->Asset, LexToString(Header.Type))
    }

    LTextureView View{Assets.PullAsset<LTextureView>(this->Asset)};
    LTexture2Ref Texture{Textures.FromTextureView(View)};

    this->Path = this->Asset.string();
    this->Texture = View.Texture;
    this->Format = vk::to_string(View.Format);
    this->MipLevels = View.MipLevels ? std::to_string(*View.MipLevels) : "<auto>";
    this->MaxSampleCount = View.MaxSampleCount ? vk::to_string(*View.MaxSampleCount) : "<auto>";

    BeginStyling(*this).StaticRoot<WRegion>()
        .Anchor(EAnchor::Fill)
        .Tint(*Prefs.ForegroundColor)
    [
        NewStaticNode(WEditorBackground)
        [
            NewStaticNode(WRegion)
                .MinDesiredSize(256_spt2)
                .Texture(Texture)
            + NewStaticNode(WEditorCategorySeparator, "Properties")
            [
                  GetEditorNode<LString>({.Viewport=this->GetViewport(), .Field=this->Path, .Default=this->Path, .What="Path",})
                    // TODO: This is of course just temporary. Later on we have to enable editing.
                    .Delegate([](auto& F){ ToggleEditorNodesTransitively(F.GetRawNode(), false); })
                + GetEditorNode<LString>({.Viewport=this->GetViewport(), .Field=this->Texture, .Default=this->Texture, .What="Texture",})
                    .Delegate([](auto& F){ ToggleEditorNodesTransitively(F.GetRawNode(), false); })
                + GetEditorNode<LString>({.Viewport=this->GetViewport(), .Field=this->Format, .Default=this->Format, .What="Format",})
                    .Delegate([](auto& F){ ToggleEditorNodesTransitively(F.GetRawNode(), false); })
                + GetEditorNode<LString>({.Viewport=this->GetViewport(), .Field=this->MipLevels, .Default=this->MipLevels, .What="Mip Levels",})
                    .Delegate([](auto& F){ ToggleEditorNodesTransitively(F.GetRawNode(), false); })
                + GetEditorNode<LString>({.Viewport=this->GetViewport(), .Field=this->MaxSampleCount, .Default=this->MaxSampleCount, .What="Samples",})
                    .Delegate([](auto& F){ ToggleEditorNodesTransitively(F.GetRawNode(), false); })
            ]
        ]
    ];

    return;
}
