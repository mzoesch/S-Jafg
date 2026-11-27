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
#include "Widgets/Input_Boolean.h"
#include "Widgets/Input_Color.h"
#include "Widgets/Input_LargeList.h"
#include "Widgets/Input_Vector2.h"

namespace Jafg
{

namespace
{

typedef LPbrMaterialTemplate::ETextureType ETextureType;

NODISCARD constexpr LLinearColor3 ToColor(LVec3F const& Vector) noexcept
{
    return LLinearColor3{Vector.x, Vector.y, Vector.z};
}

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

//# E.g., "base_diffuse_roughness" to "Base Diffuse Roughness". Abbreviations are upper case ("specular_ior" to "Specular IOR").
NODISCARD LString MakeDisplayName(LStringView Key)
{
    LString Result;
    for (auto Part: Key | std::views::split('_'))
    {
        LString Word{Part.begin(), Part.end()};
        if (Word == "ior" || Word == "uv")
        {
            std::ranges::transform(Word, Word.begin(), [](char C) { return static_cast<char>(std::toupper(C)); });
        }
        else if (!Word.empty())
        {
            Word[0] = static_cast<char>(std::toupper(Word[0]));
        }
        Result += (Result.empty() ? "" : " ") + Word;
    }
    return Result;
}

//# What the parameter does, its values and where it comes from.
template<typename T>
NODISCARD LString MakeTooltip(TPbrParameter<T> const& Info)
{
    return algo::sprintf("{}\n{}\n{} \"{}\".", Info.Description, Info.DescribeValues()
        , Info.bOpenPbr ? "OpenPBR parameter" : "Jafg extension", Info.Key);
}

//# Infinite bounds are no bounds.
NODISCARD std::optional<f64> MakeBound(f32 Value) noexcept
{
    return std::isinf(Value) ? std::nullopt : std::optional<f64>{static_cast<f64>(Value)};
}

//# A row of a parameter. The slider spans its norm, typed values its whole range.
NODISCARD Detail::LNodeFactoryBase MakeParameterRow(LViewport& Viewport, TPbrParameter<f32> const& Info, f32& Field, f32 Default)
{
    LString const Name{MakeDisplayName(Info.Key)};
    LString const Tooltip{MakeTooltip(Info)};
    auto CreateInfo{MakeInfo(Viewport, Name, Tooltip, Field, Default, MakeBound(Info.GetMin()), MakeBound(Info.Max))};
    CreateInfo.SoftMin = MakeBound(Info.NormMin);
    CreateInfo.SoftMax = MakeBound(Info.NormMax);
    return GetEditorNode<f32>(CreateInfo);
}

NODISCARD Detail::LNodeFactoryBase MakeParameterRow(LViewport& Viewport, TPbrParameter<bool> const& Info, bool& Field, bool Default)
{
    LString const Name{MakeDisplayName(Info.Key)};
    LString const Tooltip{MakeTooltip(Info)};
    return GetEditorNode<bool>(MakeInfo(Viewport, Name, Tooltip, Field, Default));
}

//# Colors (and the other color3 parameters of OpenPBR, e.g. subsurface_radius_scale).
NODISCARD Detail::LNodeFactoryBase MakeParameterRow(LViewport& Viewport, TPbrParameter<LVec3F> const& Info, LVec3F& Field, LVec3F Default)
{
    EJxxFieldFlags const Flags{EJxxFieldBits::EditorEditable};
    struct LRowState final: LEditorRowStateExt
    {
        WInput_Color* Input{};
    };
    auto SharedState{std::make_shared<LRowState>()};

    return NewNode(Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
        .Tooltip(MakeTooltip(Info))
    [
        EditorComponentLabel(Flags, Viewport, MakeDisplayName(Info.Key), SharedState)
        + EditorComponentContentWrapper(Flags, Viewport, SharedState)
        [
            NewNode(Viewport).Class<WInput_Color>(ToColor(Field)).SaveTo(&SharedState->Input)
                .Anchor(EAnchor::HFill)
                .Min(MakeBound(Info.GetMin()))
                .Max(MakeBound(Info.Max))
                .SoftMin(MakeBound(Info.NormMin))
                .SoftMax(MakeBound(Info.NormMax))
                .OnVectorChanged([SharedState, Default, Field=&Field](WInput_Color& Self)
                {
                    *Field = Self.Get<LLinearColor3>().ToDeviceReadyVector3();
                    check(SharedState->ResetButton)
                    SharedState->ResetButton->SetEnabled(*Field != Default);
                })
        ]
        + EditorResetButton(Flags, Viewport, Field != Default, [SharedState, Default](WButton& Self)
        {
            check(SharedState->Input)
            SharedState->Input->Set(ToColor(Default));
        }, LEditorRowState::Lambda<WButton>(SharedState)).SaveTo(&SharedState->ResetButton)
    ];
}

//# A row of a single bit of the flags.
NODISCARD Detail::LNodeFactoryBase MakeFlagRow(LViewport& Viewport, LString Name, LString Tooltip, EPbrMaterialFlags& Flags, EPbrMaterialBits Bit, EPbrMaterialFlags Default)
{
    EJxxFieldFlags const FieldFlags{EJxxFieldBits::EditorEditable};
    struct LRowState final: LEditorRowStateExt
    {
        WInput_Boolean* Input{};
    };
    auto SharedState{std::make_shared<LRowState>()};
    bool const bDefault{!!(Default & Bit)};

    return NewNode(Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
        .Tooltip(std::move(Tooltip))
    [
        EditorComponentLabel(FieldFlags, Viewport, std::move(Name), SharedState)
        + EditorComponentContentWrapper(FieldFlags, Viewport, SharedState)
        [
            NewNode(Viewport).Class<WInput_Boolean>().SaveTo(&SharedState->Input)
                .Anchor(EAnchor::HFill)
                .Value(!!(Flags & Bit))
                .OnValueChanged([SharedState, bDefault, Bit, Flags=&Flags](WInput_Boolean& Self)
                {
                    if (Self.GetValue())
                    {
                        *Flags |= Bit;
                    }
                    else
                    {
                        *Flags &= ~EPbrMaterialFlags{Bit};
                    }
                    check(SharedState->ResetButton)
                    SharedState->ResetButton->SetEnabled(Self.GetValue() != bDefault);
                })
        ]
        + EditorResetButton(FieldFlags, Viewport, !!(Flags & Bit) != bDefault, [SharedState, bDefault](WButton& Self)
        {
            check(SharedState->Input)
            SharedState->Input->SetValue(bDefault);
        }, LEditorRowState::Lambda<WButton>(SharedState)).SaveTo(&SharedState->ResetButton)
    ];
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
    if (this->Instance && this->Edit && LEditState::FromDevice(this->Instance->GetData()) != *this->Edit)
    {
        this->Edit->ToDevice(this->Instance->MakeDirty());
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

    this->Edit = std::make_unique<LEditState>(LEditState::FromDevice(this->Instance->GetData()));
    auto& E{*this->Edit};
    LEditState const& T{Template};
    auto& S{*this->Subsystem};

    /* A row for the parameter of the info. The default is the value of the template. */
    #define JAFG_PBR_ROW(Member) MakeParameterRow(Viewport, LPbrMaterialParameters::Member##Info, E.Member, T.Member)
    #define JAFG_PBR_TEXTURE_ROW(Type, Name, Tooltip) MakeTextureRow(Viewport, S, this->Instance, Namespace, ETextureType::Type, Name, Tooltip)

    auto Factory{
        NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Material"})
        [
            MakeTextRow(Viewport, "Name", "The pbr material. Edits are visible to everything that is drawn with it, but are not saved.", Name)
            + MakeTextRow(Viewport, "Base Material", "The material that the pbr material is drawn with.", BaseMaterial)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Base"})
        [
            JAFG_PBR_ROW(BaseWeight)
            + JAFG_PBR_ROW(BaseColor)
            + JAFG_PBR_ROW(BaseMetalness)
            + JAFG_PBR_ROW(BaseDiffuseRoughness)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Specular"})
        [
            JAFG_PBR_ROW(SpecularWeight)
            + JAFG_PBR_ROW(SpecularColor)
            + JAFG_PBR_ROW(SpecularRoughness)
            + JAFG_PBR_ROW(SpecularRoughnessAnisotropy)
            + JAFG_PBR_ROW(SpecularIor)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Transmission"})
        [
            JAFG_PBR_ROW(TransmissionWeight)
            + JAFG_PBR_ROW(TransmissionColor)
            + JAFG_PBR_ROW(TransmissionDepth)
            + JAFG_PBR_ROW(TransmissionScatter)
            + JAFG_PBR_ROW(TransmissionScatterAnisotropy)
            + JAFG_PBR_ROW(TransmissionDispersionScale)
            + JAFG_PBR_ROW(TransmissionDispersionAbbeNumber)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Subsurface"})
        [
            JAFG_PBR_ROW(SubsurfaceWeight)
            + JAFG_PBR_ROW(SubsurfaceColor)
            + JAFG_PBR_ROW(SubsurfaceRadius)
            + JAFG_PBR_ROW(SubsurfaceRadiusScale)
            + JAFG_PBR_ROW(SubsurfaceScatterAnisotropy)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Coat"})
        [
            JAFG_PBR_ROW(CoatWeight)
            + JAFG_PBR_ROW(CoatColor)
            + JAFG_PBR_ROW(CoatRoughness)
            + JAFG_PBR_ROW(CoatRoughnessAnisotropy)
            + JAFG_PBR_ROW(CoatIor)
            + JAFG_PBR_ROW(CoatDarkening)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Fuzz"})
        [
            JAFG_PBR_ROW(FuzzWeight)
            + JAFG_PBR_ROW(FuzzColor)
            + JAFG_PBR_ROW(FuzzRoughness)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Emission"})
        [
            JAFG_PBR_ROW(EmissionLuminance)
            + JAFG_PBR_ROW(EmissionColor)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Thin Film"})
        [
            JAFG_PBR_ROW(ThinFilmWeight)
            + JAFG_PBR_ROW(ThinFilmThickness)
            + JAFG_PBR_ROW(ThinFilmIor)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Geometry"})
        [
            JAFG_PBR_ROW(GeometryOpacity)
            + JAFG_PBR_ROW(GeometryThinWalled)
            + JAFG_PBR_ROW(NormalScale)
            + JAFG_PBR_ROW(CoatNormalScale)
            + JAFG_PBR_ROW(GeometryTangentRotation)
            + JAFG_PBR_ROW(GeometryCoatTangentRotation)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Jafg"})
        [
            JAFG_PBR_ROW(OcclusionFactor)
            + JAFG_PBR_ROW(AlphaCutoff)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Textures"})
        [
            JAFG_PBR_TEXTURE_ROW(BaseColor, "Base Color Map", "rgb: base_color. a: geometry_opacity (if alpha masked or blended).")
            + JAFG_PBR_TEXTURE_ROW(Normal, "Normal Map", "geometry_normal in tangent space. OpenGl standard unless DirectX Normals is set.")
            + JAFG_PBR_TEXTURE_ROW(Arm, "ARM Map", "r: Ambient occlusion. g: specular_roughness. b: base_metalness.")
            + JAFG_PBR_TEXTURE_ROW(Emission, "Emission Map", "rgb: emission_color.")
            + JAFG_PBR_TEXTURE_ROW(Specular, "Specular Map", "rgb: specular_color. a: specular_weight.")
            + JAFG_PBR_TEXTURE_ROW(Anisotropy, "Anisotropy Map", "rg: Direction of the anisotropy in tangent space (geometry_tangent), [-1,1] mapped to [0,1]. b: specular_roughness_anisotropy.")
            + JAFG_PBR_TEXTURE_ROW(ThinFilm, "Thin Film Map", "r: thin_film_weight. g: thin_film_thickness.")
            + JAFG_PBR_TEXTURE_ROW(Subsurface, "Subsurface Map", "rgb: subsurface_color. a: subsurface_weight.")
            + JAFG_PBR_TEXTURE_ROW(CoatNormal, "Coat Normal Map", "geometry_coat_normal in tangent space. Same standard as the normal map.")
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Sampling"})
        [
            GetEditorNode<UBO::Bindless::Sampler>(MakeInfo(Viewport, "Sampler", "How the textures are sampled.", E.Sampler, T.Sampler))
            + MakeVec2Row(Viewport, "UV Scale", MakeTooltip(LPbrMaterialParameters::UVScaleInfo), E.UVScale, T.UVScale)
            + MakeVec2Row(Viewport, "UV Offset", MakeTooltip(LPbrMaterialParameters::UVOffsetInfo), E.UVOffset, T.UVOffset)
        ]
        + NewNode(Viewport).Class<WEditorCategorySeparator>(LString{"Flags"})
        [
            MakeFlagRow(Viewport, "Alpha Mask", "Discard where the coverage (geometry_opacity times the alpha of the base color map) is below the alpha cutoff.", E.Flags, EPbrMaterialBits::AlphaMask, T.Flags)
            + MakeFlagRow(Viewport, "Alpha Blend", "Blend by the coverage (geometry_opacity times the alpha of the base color map), even if geometry_opacity is one.", E.Flags, EPbrMaterialBits::AlphaBlend, T.Flags)
            + MakeFlagRow(Viewport, "Double Sided", "Draw both faces. Back faces are shaded with flipped normals. Implied by geometry_thin_walled.", E.Flags, EPbrMaterialBits::DoubleSided, T.Flags)
            + MakeFlagRow(Viewport, "DirectX Normals", "The normal maps use the DirectX standard (Y flipped).", E.Flags, EPbrMaterialBits::DirectXNormals, T.Flags)
        ]
        };

    #undef JAFG_PBR_TEXTURE_ROW
    #undef JAFG_PBR_ROW

    TArray<TJxxUnique<WNode>> Xs;
    this->Container->AddChild(std::move(Factory).UniqueXs(&Xs));
    for (auto& X: Xs)
    {
        this->Container->AddChild(std::move(X));
    }
}
