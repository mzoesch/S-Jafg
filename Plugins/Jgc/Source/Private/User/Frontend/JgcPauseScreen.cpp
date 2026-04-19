// Copyright mzoesch. All rights reserved.

#include "User/Frontend/JgcPauseScreen.h"
#include "Framework/Frontend.h"
#include "Nodes/Region.h"
#include "Nodes/Box.h"
#include "Nodes/TextBox.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/FontSubsystem.h"

void Jgc::WPauseScreen::Construct()
{
    Super::Construct();

    auto& Frontend{this->GetFrontend()};

    auto& TextureSubsystem{*Frontend.GetSubsystemChecked<Jafg::JTextureSubsystem>()};

    this->Padding = 5;

    // auto Tex = TextureSubsystem.FromTextureViewIdentifier("Jafg.Test");

    // Frontend.Vk_AddTextureToGlobalBindlessArray(&*Tex);

    Jafg::BeginStyling(*this).StaticRoot<Jafg::WOverlay>()
        .Anchor(Jafg::EAnchor::CenterCenter)
        .MinDesiredSize({200_pt, 200})
    [
        NewStaticNode(Jafg::WTextBox)
            .Anchor(Jafg::EAnchor::CenterCenter)
            .Tint(Colors::Green)
            .BackgroundTint(Colors::Red)
            .OutlineTint(Colors::Blue)
            .MinDesiredSize({400_pt, 150})
            .TextAlign(Jafg::ETextHAlign::Right)
            .TextAlign(Jafg::ETextVAlign::Bottom)
            .Content("hioasdfphjkoasdfhpjoasdfhpjasdfhjp")
    ];

    // Jafg::BeginStyling(*this).Root<Jafg::WRegion>()
    //     .Anchor(Jafg::EAnchor::CenterCenter)
    //     .MinDesiredSize({100_pt, 100})
    //     .Tint(Colors::DarkerGray)
    // [
    //     NewStaticNode(Jafg::WTextBox)
    //         .OutlineThickness(2)
    //         .OutlineTint(Colors::Indigo)
    //         .Tint(Colors::DarkerGray)
    //         .MinDesiredSize({25_pt2})
    //         .Content("Hello, World!")
    // ];

    // Jafg::BeginStyling(*this).Root<Jafg::WRegion>()
    //     .Anchor(Jafg::EAnchor::TopRight)
    //     .MinDesiredSize({192_spt, 108})
    //     .TexCoordBehavior(Jafg::ETexCoordBehavior::Scale)
    //     .Texture(Tex)
    //     .SamplerAddressMode(vk::SamplerAddressMode::eRepeat)
    //     .BackgroundTint(Colors::Red)
    //     ;
    //
    // Jafg::BeginStyling(*this).Root<Jafg::WRegion>()
    //     .Anchor(Jafg::EAnchor::CenterLeft)
    //     .MinDesiredSize({100_spt, 200})
    //     .TexCoordBehavior(Jafg::ETexCoordBehavior::Scale)
    //     .Texture(Tex)
    //     .SamplerAddressMode(vk::SamplerAddressMode::eRepeat)
    //     .BackgroundTint(Colors::Red)
    //     ;
    //

    // Jafg::BeginStyling(*this).Root<Jafg::WRegion>()
    //     .Anchor(Jafg::EAnchor::BottomLeft)
    //     .MinDesiredSize({200_pt, 300})
    //     .TexCoordBehavior(Jafg::ETexCoordBehavior::FitH)
    //     .Texture(Tex)
    //     .SamplerAddressMode(vk::SamplerAddressMode::eClampToEdge)
    //     .BackgroundTint(Colors::Red)
    //     ;
    //
    // Jafg::BeginStyling(*this).Root<Jafg::WRegion>()
    //     .Anchor(Jafg::EAnchor::CenterRight)
    //     .MinDesiredSize({300_pt, 100})
    //     .TexCoordBehavior(Jafg::ETexCoordBehavior::FitV)
    //     .Texture(Tex)
    //     .SamplerAddressMode(vk::SamplerAddressMode::eClampToBorder)
    //     .BackgroundTint(Colors::Wheat)
    //     ;
    // Jafg::BeginStyling(*this).Root<Jafg::WRegion>()
    //     .Anchor(Jafg::EAnchor::BottomRight)
    //     .MinDesiredSize({300_pt, 100})
    //     .TexCoordBehavior(Jafg::ETexCoordBehavior::FitH)
    //     .Texture(Tex)
    //     .SamplerAddressMode(vk::SamplerAddressMode::eMirroredRepeat)
    //     .BackgroundTint(Colors::Red)
        ;

    return;
}
