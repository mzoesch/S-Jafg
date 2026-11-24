// Copyright mzoesch. All rights reserved.

#include "Widgets/PbrMaterialInspector.h"
#include "Engine/Engine.h"
#include "Framework/Editor.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/Surface.h"
#include "Nodes/EditableTextButton.h"
#include "Nodes/HParent.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "Nodes/VParent.h"
#include "Nodes/VRegion.h"
#include "Nodes/Viewport.h"
#include "User/UserPreferences.h"
#include "Widgets/EditorFactory.h"
#include "Widgets/Input_LargeList.h"
#include "Widgets/Input_Vector2.h"

namespace Jafg
{

namespace
{

typedef LPbrMaterialTemplate::ETextureType ETextureType;

template<typename T>
NODISCARD TEditorNodeCreateInfo<T> MakeInfo(LViewport& Viewport, LStringView Name, LStringView Tooltip, T& Field, std::type_identity_t<T> Default
    , std::optional<f64> Min = {}
    , std::optional<f64> Max = {}
    )
{
    return TEditorNodeCreateInfo<T>{
        .Viewport = Viewport,
        .Owner = nullptr,
        .Flags = EJxxFieldBits::EditorEditable,
        .RecordName = Name,
        .RecordTooltip = Tooltip,
        .Field = Field,
        .Default = std::move(Default),
        .Min = Min,
        .Max = Max,
        };
}

//# A row that only shows a text.
NODISCARD Detail::LNodeFactoryBase MakeTextRow(LViewport& Viewport, LString Name, LString Tooltip, LString Content)
{
    EJxxFieldFlags const Flags{EJxxFieldBits::EditorVisible};
    auto SharedState{std::make_shared<LEditorRowState>()};

    return NewNode(Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
        .Tooltip(std::move(Tooltip))
    [
        EditorComponentLabel(Flags, Viewport, std::move(Name), SharedState)
        + EditorComponentContentWrapper(Flags, Viewport, SharedState)
        [
            NewNode(Viewport).Class<WText>()
                .Anchor(EAnchor::HFill)
                .TextTint(Colors::Gray)
                .Content(std::move(Content))
        ]
        + EditorResetButton(Flags, Viewport, false, [](WButton& Self) {}, LEditorRowState::Lambda<WButton>(SharedState))
    ];
}

NODISCARD Detail::LNodeFactoryBase MakeVec2Row(LViewport& Viewport, LString Name, LString Tooltip, LVec2F& Field, LVec2F Default)
{
    EJxxFieldFlags const Flags{EJxxFieldBits::EditorEditable};
    struct LRowState final: LEditorRowStateExt
    {
        WInput_Vector2* Input{};
    };
    auto SharedState{std::make_shared<LRowState>()};

    return NewNode(Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
        .Tooltip(std::move(Tooltip))
    [
        EditorComponentLabel(Flags, Viewport, std::move(Name), SharedState)
        + EditorComponentContentWrapper(Flags, Viewport, SharedState)
        [
            NewNode(Viewport).Class<WInput_Vector2>(LVec2D{Field}).SaveTo(&SharedState->Input)
                .Anchor(EAnchor::HFill)
                .OnVectorChanged([SharedState, Default, Field=&Field](WInput_Vector2& Self)
                {
                    *Field = LVec2F{Self.Get<f64>()};
                    check(SharedState->ResetButton)
                    SharedState->ResetButton->SetEnabled(*Field != Default);
                })
        ]
        + EditorResetButton(Flags, Viewport, Field != Default, [SharedState, Default](WButton& Self)
        {
            check(SharedState->Input)
            SharedState->Input->Set(LVec2D{Default});
        }, LEditorRowState::Lambda<WButton>(SharedState)).SaveTo(&SharedState->ResetButton)
    ];
}

//#
//# A row that shows the texture of a type of the instance. It can be browsed for (any texture inside of a content
//# dir), cleared or reset to the texture of the template.
//#
NODISCARD Detail::LNodeFactoryBase MakeTextureRow(LViewport& Viewport
    , JMaterialSubsystem& Subsystem
    , LPbrMaterialInstanceRef const& Instance
    , LPluginInfo Namespace
    , ETextureType Type
    , LString Name
    , LString Tooltip
    )
{
    check(Instance && Namespace)
    auto& Prefs{GetSingleton<JUserPreferences>()};
    EJxxFieldFlags const Flags{EJxxFieldBits::EditorEditable};

    struct LRowState final: LEditorRowStateExt
    {
        JMaterialSubsystem* Subsystem{};
        LPbrMaterialInstanceRef Instance;
        //# Of the template.
        LPluginInfo Namespace{ nullptr };
        ETextureType Type{};
        //# Relative to #Namespace. Empty if the template has none.
        LString TemplateTexture;

        WEditableTextButton* Path{};
        WTextButtonIconizedDouble* Clear{};

        NODISCARD LTexture2 const* GetCurrent() const noexcept
        {
            return this->Subsystem->FindPbrTexture(*this->Instance, this->Type);
        }
        NODISCARD bool IsTemplate() const
        {
            LTexture2 const* const Current{this->GetCurrent()};
            if (this->TemplateTexture.empty())
            {
                return !Current;
            }
            return Current && Current->GetPath().lexically_normal() == (*this->Namespace / this->TemplateTexture).lexically_normal();
        }
        NODISCARD static LString Describe(LTexture2 const* Texture)
        {
            if (!Texture)
            {
                return "<none>";
            }
            if (Detail::LPluginInfoRegistry::FindContaining(Texture->GetPath()))
            {
                auto const [TextureNamespace, Path]{SplitIntoNamespace(Texture->GetPath())};
                return GetEditorResourceName(TextureNamespace, Path);
            }
            return Texture->GetPath().generic_string();
        }
        void Refresh()
        {
            check(this->Path && this->Clear && this->ResetButton)
            if (LString Content{Describe(this->GetCurrent())}; Content != this->Path->GetContent())
            {
                this->Path->SetContent(std::move(Content));
            }
            if (bool const bResettable{!this->IsTemplate()}; bResettable != this->ResetButton->IsEnabled())
            {
                this->ResetButton->SetEnabled(bResettable);
            }
            if (bool const bClearable{!!this->GetCurrent()}; bClearable != this->Clear->IsEnabled())
            {
                this->Clear->SetEnabled(bClearable);
            }
        }
        void Set(LTexture2Ref Texture)
        {
            this->Subsystem->SetPbrTexture(*this->Instance, this->Type, std::move(Texture));
            this->Refresh();
        }
    };

    auto SharedState{std::make_shared<LRowState>()};
    SharedState->Subsystem = &Subsystem;
    SharedState->Instance = Instance;
    SharedState->Namespace = Namespace;
    SharedState->Type = Type;
    SharedState->TemplateTexture = Subsystem.ResolvePbrTemplateTexture(Namespace, Instance->Template, Type);

    LTexture2 const* const Current{SharedState->GetCurrent()};
    return NewNode(Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
        .Tooltip(std::move(Tooltip))
    [
        EditorComponentLabel(Flags, Viewport, std::move(Name), SharedState)
        + EditorComponentContentWrapper(Flags, Viewport, SharedState)
        [
            NewNode(Viewport).Class<WHParent>()
                .Anchor(EAnchor::Fill)
            [
                NewNode(Viewport).Class<WEditableTextButton>().SaveTo(&SharedState->Path)
                    .Anchor(EAnchor::HFill)
                    .Palette(*Prefs.InputPaletteSolid)
                    .Content(LRowState::Describe(Current))
                    .OnContentCommitted([SharedState](WEditableTextButton& Self, LString const& Content, ETextCommit Commit)
                    {
                        /* Only shows the texture (and lets it be copied). Typed text is discarded. */
                        Self.SetContent(LRowState::Describe(SharedState->GetCurrent()));
                    })
                + NewNode(Viewport).Class<WTextButtonIconizedDouble>()
                    .Palette(*Prefs.SecondaryPaletteSolid)
                    .InAllBrushes<&LBoxBrush::Padding>(Prefs.InputPadding)
                    .Content("Browse")
                    .LeftIcon({JafgInfo, "Icons/DirectorySearch"})
                    .OnKeyEventFocused([SharedState](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (!Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton) || !Self.AabbTest(Info))
                        {
                            return LNodeReply::Unhandled();
                        }

                        LPath DefaultPath{**SharedState->Namespace};
                        if (LTexture2 const* const Texture{SharedState->GetCurrent()}; Texture && Texture->GetPath().has_parent_path())
                        {
                            DefaultPath = Texture->GetPath().parent_path();
                        }

                        auto const AbsolutePath{Info.Viewport.GetSurface().OpenBlockingDialogForFile({
                            .Filters = {FileDialogFilterPresets::Textures}
                            }, DefaultPath)};
                        if (!AbsolutePath)
                        {
                            LOG_INFO(LogEditor, "Texture selection cancelled. No path provided. Discarding request.")
                            return LNodeReply::Handled();
                        }
                        if (!Detail::LPluginInfoRegistry::FindContaining(*AbsolutePath))
                        {
                            LOG_ERROR(LogEditor, "[{}]: Path is not inside the content dir of any namespace. Discarding request.", *AbsolutePath)
                            return LNodeReply::Handled();
                        }
                        auto const [TextureNamespace, Path]{SplitIntoNamespace(*AbsolutePath)};
                        SharedState->Set(SharedState->Subsystem->LoadPbrTexture(TextureNamespace, Path, SharedState->Type));
                        return LNodeReply::Handled();
                    })
                + NewNode(Viewport).Class<WTextButtonIconizedDouble>().SaveTo(&SharedState->Clear)
                    .Palette(*Prefs.SecondaryPaletteSolid)
                    .InAllBrushes<&LBoxBrush::Padding>(Prefs.InputPadding)
                    .Content("Clear")
                    .LeftIcon({JafgInfo, "Icons/X"})
                    .Enabled(!!Current)
                    .OnKeyEventFocused([SharedState](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton) && Self.AabbTest(Info))
                        {
                            SharedState->Set(nullptr);
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
            ]
        ]
        + EditorResetButton(Flags, Viewport, !SharedState->IsTemplate(), [SharedState](WButton& Self)
        {
            SharedState->Set(SharedState->TemplateTexture.empty()
                ? LTexture2Ref{}
                : SharedState->Subsystem->LoadPbrTexture(SharedState->Namespace, SharedState->TemplateTexture, SharedState->Type));
        }, LEditorRowState::Lambda<WButton>(SharedState)).SaveTo(&SharedState->ResetButton)
    ];
}

} /* ~Namespace <Anonymous> */

} /* ~Namespace Jafg */

Jafg::WPbrMaterialInspector::LEditState Jafg::WPbrMaterialInspector::LEditState::FromData(SSBO::BindlessPbrMaterial const& Data) noexcept
{
    return {
        .AlbedoFactor = LLinearColor{Data.AlbedoFactor.x, Data.AlbedoFactor.y, Data.AlbedoFactor.z, Data.AlbedoFactor.w},
        .OcclusionFactor = Data.OcclusionFactor,
        .RoughnessFactor = Data.RoughnessFactor,
        .MetallicFactor = Data.MetallicFactor,
        .EmissiveFactor = LLinearColor3{Data.EmissiveFactor.x, Data.EmissiveFactor.y, Data.EmissiveFactor.z},
        .AlphaCutoff = Data.AlphaCutoff,
        .NormalScale = Data.NormalScale,
        .Sampler = Data.Sampler,
        .UVScale = Data.UVScale,
        .UVOffset = Data.UVOffset,
        .bAlphaMask = !!(Data.Flags & EPbrMaterialBits::AlphaMask),
        .bDoubleSided = !!(Data.Flags & EPbrMaterialBits::DoubleSided),
        .bDirectXNormals = !!(Data.Flags & EPbrMaterialBits::DirectXNormals),
        };
}

Jafg::WPbrMaterialInspector::LEditState Jafg::WPbrMaterialInspector::LEditState::FromTemplate(LPbrMaterialTemplate const& Template) noexcept
{
    return {
        .AlbedoFactor = Template.AlbedoFactor,
        .OcclusionFactor = Template.OcclusionFactor,
        .RoughnessFactor = Template.RoughnessFactor,
        .MetallicFactor = Template.MetallicFactor,
        .EmissiveFactor = LLinearColor3{Template.EmissiveFactor.x, Template.EmissiveFactor.y, Template.EmissiveFactor.z},
        .AlphaCutoff = Template.AlphaCutoff,
        .NormalScale = Template.NormalScale,
        .Sampler = Template.Sampler,
        .UVScale = Template.UVScale,
        .UVOffset = Template.UVOffset,
        .bAlphaMask = !!(Template.Flags & EPbrMaterialBits::AlphaMask),
        .bDoubleSided = !!(Template.Flags & EPbrMaterialBits::DoubleSided),
        .bDirectXNormals = !!(Template.Flags & EPbrMaterialBits::DirectXNormals),
        };
}

void Jafg::WPbrMaterialInspector::LEditState::ApplyTo(SSBO::BindlessPbrMaterial& Data) const noexcept
{
    Data.AlbedoFactor = this->AlbedoFactor.ToDeviceReadyVector4();
    Data.OcclusionFactor = this->OcclusionFactor;
    Data.RoughnessFactor = this->RoughnessFactor;
    Data.MetallicFactor = this->MetallicFactor;
    Data.EmissiveFactor = this->EmissiveFactor.ToDeviceReadyVector3();
    Data.AlphaCutoff = this->AlphaCutoff;
    Data.NormalScale = this->NormalScale;
    Data.Sampler = this->Sampler;
    Data.UVScale = this->UVScale;
    Data.UVOffset = this->UVOffset;

    EPbrMaterialFlags Flags{EPbrMaterialBits::Identity};
    if (this->bAlphaMask)
    {
        Flags |= EPbrMaterialBits::AlphaMask;
    }
    if (this->bDoubleSided)
    {
        Flags |= EPbrMaterialBits::DoubleSided;
    }
    if (this->bDirectXNormals)
    {
        Flags |= EPbrMaterialBits::DirectXNormals;
    }
    Data.Flags = Flags;
}

void Jafg::WPbrMaterialInspector::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JUserPreferences>()};
    this->Subsystem = this->GetViewport().GetSurface().GetMutableFrontend().GetSubsystemChecked<JMaterialSubsystem>();

    this->Entries.clear();
    for (auto const& [Namespace, Templates]: this->Subsystem->GetPbrTemplates())
    {
        for (auto const& Template: Templates)
        {
            this->Entries.push_back({.Name = GetEditorResourceName(Namespace, Template.Identifier), .Namespace = Namespace, .Identifier = Template.Identifier});
        }
    }
    std::ranges::sort(this->Entries, algo::lexicographical_string_compare, &LEntry::Name);

    TArray<LInputListItem> Items;
    Items.reserve(this->Entries.size());
    for (auto const& Entry: this->Entries)
    {
        Items.emplace_back(Entry.Name);
    }

    BeginStyling(*this).StaticRoot<WVRegion>()
        .Anchor(EAnchor::Fill)
        .Tint(*Prefs.ForegroundColor)
    [
        NewStaticNode(WHParent)
            .Anchor(EAnchor::HFill)
            .Padding({5_spt})
            .Space(5_spt)
        [
            NewStaticNode(WInput_LargeList).SaveTo(&this->Selector)
                .Anchor(EAnchor::HFill)
                .Content("Select a pbr material...")
                .Items(std::move(Items))
                .OnValueChanged([this](LInputListItem const& Selected)
                {
                    auto const It{std::ranges::find(this->Entries, Selected.Identifier, &LEntry::Name)};
                    check(It != this->Entries.end())
                    this->Inspect(this->Subsystem->GetPbrInstance(It->Namespace, It->Identifier));
                })
        ]
        + NewStaticNode(WScrollRegion)
            .Anchor(EAnchor::Fill)
            .SkipBrushDraw(true)
        [
            NewStaticNode(WVParent).SaveTo(&this->Container)
                .Anchor(EAnchor::HFill)
        ]
    ];

    this->Rebuild();
}

void Jafg::WPbrMaterialInspector::Tick(f32 Dt)
{
    Super::Tick(Dt);

    /* Pushes the edits of the rows. The data is shared by everything that is drawn with the instance. */
    if (this->Instance && this->Edit && LEditState::FromData(this->Instance->GetData()) != *this->Edit)
    {
        this->Edit->ApplyTo(this->Instance->MakeDirty());
    }
}

void Jafg::WPbrMaterialInspector::Inspect(LPbrMaterialInstanceRef InInstance)
{
    this->Instance = std::move(InInstance);
    /* Not constructed yet (e.g., a new tab). Then the construction builds the rows. */
    if (this->Container)
    {
        this->Rebuild();
    }
}

void Jafg::WPbrMaterialInspector::Rebuild()
{
    check(this->Container && this->Selector && this->Subsystem)

    /* First the rows. They point into the edit state. */
    this->Container->RemoveChildren();
    this->Edit.reset();

    auto& Viewport{this->GetViewport()};

    if (!this->Instance)
    {
        this->Selector->SetContent("Select a pbr material...");
        this->Container->AddChild(NewNode(Viewport).Class<WText>()
            .Anchor(EAnchor::HFill)
            .Padding({5_spt})
            .TextTint(Colors::Gray)
            .Content("Select a pbr material to inspect it. Or inspect one from the details of a static mesh.")
            .Unique());
        return;
    }

    auto const& Template{this->Instance->Template};
    LPluginInfo const Namespace{this->Subsystem->FindPbrTemplateNamespace(Template)};
    check(Namespace)
    LString const Name{GetEditorResourceName(Namespace, Template.Identifier)};
    this->Selector->SetContent(Name);

    LString BaseMaterial{"<none>"};
    if (this->Instance->Instance && this->Instance->Instance->Material)
    {
        auto const& BaseTemplate{this->Instance->Instance->Material->Template};
        BaseMaterial = BaseTemplate.identifier;
        for (auto const& [BaseNamespace, Templates]: this->Subsystem->GetTemplates())
        {
            if (algo::any_of(Templates, [&BaseTemplate](auto const& Candidate) { return &Candidate == &BaseTemplate; }))
            {
                BaseMaterial = GetEditorResourceName(BaseNamespace, BaseTemplate.identifier);
                break;
            }
        }
    }

    this->Edit = std::make_unique<LEditState>(LEditState::FromData(this->Instance->GetData()));
    auto& E{*this->Edit};
    LEditState const T{LEditState::FromTemplate(Template)};
    auto& S{*this->Subsystem};

    auto Factory{
        NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Material"})
        [
            MakeTextRow(Viewport, "Name", "The pbr material. Edits are visible to everything that is drawn with it, but are not saved.", Name)
            + MakeTextRow(Viewport, "Base Material", "The material that the pbr material is drawn with.", BaseMaterial)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Factors"})
        [
            GetEditorNode<LLinearColor>(MakeInfo(Viewport, "Albedo", "Multiplied with the albedo texture, or the base color without one.", E.AlbedoFactor, T.AlbedoFactor, 0.0, 1.0))
            + GetEditorNode<f32>(MakeInfo(Viewport, "Occlusion", "Range [0,1] with 0 = completely occluded and 1 = lit.", E.OcclusionFactor, T.OcclusionFactor, 0.0, 1.0))
            + GetEditorNode<f32>(MakeInfo(Viewport, "Roughness", "Range [0,1] with 0 = smooth and 1 = rough.", E.RoughnessFactor, T.RoughnessFactor, 0.0, 1.0))
            + GetEditorNode<f32>(MakeInfo(Viewport, "Metallic", "Range [0,1] with 0 = dielectric and 1 = metallic.", E.MetallicFactor, T.MetallicFactor, 0.0, 1.0))
            + GetEditorNode<LLinearColor3>(MakeInfo(Viewport, "Emissive", "The color of the emission. Multiplied with the emissive texture. Not bound above: Lights are physical, so emission may be brighter than 1.", E.EmissiveFactor, T.EmissiveFactor, 0.0))
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Alpha & Normals"})
        [
            GetEditorNode<f32>(MakeInfo(Viewport, "Alpha Cutoff", "Discards below this alpha if alpha masked. Range [0,1].", E.AlphaCutoff, T.AlphaCutoff, 0.0, 1.0))
            + GetEditorNode<f32>(MakeInfo(Viewport, "Normal Scale", "Strength of the normal map. 0 = no normal mapping.", E.NormalScale, T.NormalScale, 0.0))
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Textures"})
        [
            MakeTextureRow(Viewport, S, this->Instance, Namespace, ETextureType::Albedo, "Albedo Map", "The base color map.")
            + MakeTextureRow(Viewport, S, this->Instance, Namespace, ETextureType::Normal, "Normal Map", "The normal map. OpenGl standard unless DirectX Normals is set.")
            + MakeTextureRow(Viewport, S, this->Instance, Namespace, ETextureType::Arm, "ARM Map", "R: Ambient occlusion, G: Roughness, B: Metallic.")
            + MakeTextureRow(Viewport, S, this->Instance, Namespace, ETextureType::Emissive, "Emissive Map", "The emissive map.")
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Sampling"})
        [
            GetEditorNode<UBO::Bindless::Sampler>(MakeInfo(Viewport, "Sampler", "How the textures are sampled.", E.Sampler, T.Sampler))
            + MakeVec2Row(Viewport, "UV Scale", "Multiplied with the texture coordinates.", E.UVScale, T.UVScale)
            + MakeVec2Row(Viewport, "UV Offset", "Added to the texture coordinates.", E.UVOffset, T.UVOffset)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Flags"})
        [
            GetEditorNode<bool>(MakeInfo(Viewport, "Alpha Mask", "Discard below the alpha cutoff.", E.bAlphaMask, T.bAlphaMask))
            + GetEditorNode<bool>(MakeInfo(Viewport, "Double Sided", "Flip normals on back faces. The base material has to support it.", E.bDoubleSided, T.bDoubleSided))
            + GetEditorNode<bool>(MakeInfo(Viewport, "DirectX Normals", "The normal map uses the DirectX standard (Y flipped).", E.bDirectXNormals, T.bDirectXNormals))
        ]
        };

    TArray<TJxxUnique<WNode>> Xs;
    this->Container->AddChild(std::move(Factory).UniqueXs(&Xs));
    for (auto& X: Xs)
    {
        this->Container->AddChild(std::move(X));
    }
}
