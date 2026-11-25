// Copyright mzoesch. All rights reserved.

#include "User/UserPreferences.h"
#include "Framework/Frontend.h"
#include "User/PreferenceCollection.h"
#include "User/PreferenceValue.h"
#include "User/ControlsPreferences.h"
#include "Widgets/Input_Slider.h"
#include "Widgets/Input_List.h"
#include "Widgets/Input_Boolean.h"
#include "Nodes/EditableTextButton.h"
#include "Core/Finder2.h"
#include "Serialization/StringStatements.h"
#include "User/PreferenceCollection.h"

namespace Jafg
{

constexpr LNodeSize2 MinSize{99_spt, 0};
constexpr LNodeSize2 TextMinSize{160_spt, 0};

//# Adds the change of the preference, or updates its pending one.
static void RecordChange(TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue& Preference, LString Value)
{
    if (auto It{algo::find(Changes, &Preference, [](LPreferenceValueChangeRequest const& Change){ return &Change.Preference; })};
        It == Changes.end())
    {
        Changes.emplace_back(Preference, std::move(Value));
    }
    else
    {
        It->Value = std::move(Value);
    }
}

static LFactoryNode CreateCheckmark(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue& Preference)
{
    return NewNode(Viewport).Class<WInput_Boolean>()
        .Value(serde::from_string<bool>(Preference.GetValue()))
        .OnValueChanged([Changes=&Changes,Preference=&Preference](WInput_Boolean& Self)
        {
            RecordChange(*Changes, *Preference, serde::to_string(Self.GetValue()));
        });
}

static LFactoryNode CreateSlider(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue_Scalar& Preference)
{
    check(Preference.HasMinimum())
    check(Preference.HasMaximum())
    return NewNode(Viewport).Class<WInput_Slider>()
        .MinDesiredSize(MinSize)
        .Value(serde::from_string<f64>(Preference.GetValue()))
        .Step(Preference.GetStep())
        .Minimum(Preference.GetMinimum())
        .Maximum(Preference.GetMaximum())
        .OnValueChanged([Changes=&Changes,Preference=&Preference](f64 Value)
        {
            RecordChange(*Changes, *Preference, serde::to_string(Value));
        });
}

static LFactoryNode CreateList(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue_List& Preference)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    LString Value{Preference.GetValue()};
    TArray<LInputListItem> Items{Preference.GetItems()};
    LString Selected{Value.empty()
        ? LString{"<None>"}
        : (Value == JUserPreferences::ListAuto)
            ? "Auto"
            : Value
        };
    /* Shown as the list shows the item. */
    if (auto It{algo::find(Items, Value, &LInputListItem::Identifier)}; It != Items.end() && !It->DisplayName.empty())
    {
        Selected = It->DisplayName;
    }
    return NewNode(Viewport).Class<WInput_List>()
        .MinDesiredSize(MinSize)
        .Tint(*Prefs.InputColor)
        .OutlineThickness(1)
        .OutlineTint({0x8F})
        .Padding({5_spt, 5})
        .TextTint(Colors::White)
        .Selected(std::move(Selected))
        .Items(std::move(Items))
        .OnValueChanged([Changes=&Changes, Preference=&Preference](LInputListItem const& Value)
        {
            RecordChange(*Changes, *Preference, Value.Identifier);
        });
}

static LFactoryNode CreateTextField(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue_Text& Preference)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    return NewNode(Viewport).Class<WEditableTextButton>()
        /* Not filling the row (its default), so that it lines up with the other inputs on the right. */
        .Anchor(EAnchor::Identity)
        .MinDesiredSize(TextMinSize)
        .Palette(*Prefs.InputPaletteSolid)
        .InAllBrushes<&LBoxBrush::Padding>(Prefs.InputPadding)
        .Content(Preference.GetValue())
        .PlaceholderContent(Preference.GetPlaceholder())
        .OnContentChanged([Changes=&Changes, Preference=&Preference](WEditableTextButton&, LString const& Content)
        {
            RecordChange(*Changes, *Preference, Content);
        });
}

} /* ~Namespace <Anonymous> */

void Jafg::JUserPreferences::BeginLife()
{
    Super::BeginLife();

#define MAKE_SUBCOLLECTION(...) \
    auto& SubCollection{*static_cast<LPreferenceCollection*>(&*Collection.Children.emplace_back( \
        std::make_unique<LPreferenceCollection>(LPreferenceCollection::CreateInfo{ \
            __VA_ARGS__ \
            }) \
        ))}; \
    auto& Collection{SubCollection};
#define MAKE_BOOLEAN(Preference, ...) \
    (void)Preference; \
    Collection.Children.emplace_back(std::make_unique<LPreferenceValue_Boolean>(LPreferenceValue_Boolean::CreateInfo{ \
        __VA_ARGS__ \
        .Getter = [this]{ return JUserPreferences:: JAFG_JOIN_OUTER_TWO(_GetField_, Preference) (*this); }, \
        .Setter = [this](LString const& Value) { JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); }, \
        .CreateEditNode = CreateCheckmark, \
        }));
#define MAKE_SCALAR(Preference, ...) \
    (void)Preference; \
    Collection.Children.emplace_back(std::make_unique<LPreferenceValue_Scalar>(LPreferenceValue_Scalar::CreateInfo{ \
        __VA_ARGS__ \
        .Minimum = this-> Preference .GetMin(), \
        .Maximum = this-> Preference .GetMax(), \
        .Getter = [this]{ return JUserPreferences:: JAFG_JOIN_OUTER_TWO(_GetField_, Preference) (*this); }, \
        .Setter = [this](LString const& Value) { JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); }, \
        .CreateEditNode = CreateSlider, \
        }));
#define MAKE_SCALAR_MINMAX(Preference, ...) \
    (void)Preference; \
    Collection.Children.emplace_back(std::make_unique<LPreferenceValue_Scalar>(LPreferenceValue_Scalar::CreateInfo{ \
        __VA_ARGS__ \
        .Getter = [this]{ return JUserPreferences:: JAFG_JOIN_OUTER_TWO(_GetField_, Preference) (*this); }, \
        .Setter = [this](LString const& Value) { JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); }, \
        .CreateEditNode = CreateSlider, \
        }));
#define MAKE_LIST(Preference, ...) \
    (void)Preference; \
    Collection.Children.emplace_back(std::make_unique<LPreferenceValue_List>(LPreferenceValue_List::CreateInfo{ \
        __VA_ARGS__ \
        .Getter = [this]{ return JUserPreferences:: JAFG_JOIN_OUTER_TWO(_GetField_, Preference) (*this); }, \
        .Setter = [this](LString const& Value) \
        { \
            if (Value == ListAuto) \
            { \
                this-> Preference .Reset(); \
            } \
            else \
            { \
                JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); \
            } \
        }, \
        .CreateEditNode = CreateList, \
        }));
#define MAKE_TEXT(Preference, ...) \
    (void)Preference; \
    Collection.Children.emplace_back(std::make_unique<LPreferenceValue_Text>(LPreferenceValue_Text::CreateInfo{ \
        __VA_ARGS__ \
        .Placeholder = this-> Preference .GetDefault(), \
        .Getter = [this]{ return JUserPreferences:: JAFG_JOIN_OUTER_TWO(_GetField_, Preference) (*this); }, \
        .Setter = [this](LString const& Value) { JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); }, \
        .CreateEditNode = CreateTextField, \
        }));

    {
        auto& Collection{this->TopLevelCollections.emplace_back(LPreferenceCollection::CreateInfo{"Audio", "Audio"})};
        MAKE_SCALAR(MasterVolume,
            .Identifier = "MasterVolume",
            .DisplayName = "Master Volume",
            .Tooltip = "Volume of the application",
            .Step = 0.01,
            )
        MAKE_SCALAR(MusicVolume,
            .Identifier = "MusicVolume",
            .DisplayName = "Music Volume",
            .Tooltip = "Volume of music",
            .Step = 0.01,
            )
        MAKE_SCALAR(MiscVolume,
            .Identifier = "MiscVolume",
            .DisplayName = "Misc Volume",
            .Tooltip = "Volume of miscellaneous stuff",
            .Step = 0.01,
            )
        MAKE_SCALAR(VoiceVolume,
            .Identifier = "VoiceVolume",
            .DisplayName = "Voice Volume",
            .Tooltip = "Volume of voice",
            .Step = 0.01,
            )
    }

    {
        auto& Collection{this->TopLevelCollections.emplace_back(LPreferenceCollection::CreateInfo{"Video", "Video"})};
        MAKE_LIST(PreferredPhysicalDevice,
            .Identifier = "PhysicalDevice",
            .DisplayName = "Physical Device",
            .Tooltip = "Physical devices fulfilling the requirements of this application",
            .bRequiresRestart = true,
            .Items = [this] -> TArray<LPreferenceValue_List::value_type> {
                TArray<LPreferenceValue_List::value_type> Result;
                Result.emplace_back(ListAuto, "Auto");
                algo::for_each(this->GetFrontend().Vk_GetAvailablePhysicalDevices()
                    , [Result=&Result](LFrontend::LRankedPhysicalDevice const& Device)
                {
                    Result->emplace_back(Device.PhysicalDevice.getProperties().deviceName);
                });
                return Result;
            },)
        MAKE_LIST(PreferredPhysicalViewport,
            .Identifier = "PhysicalViewport",
            .DisplayName = "Physical Viewport",
            .Tooltip = "Physical viewports fulfilling the requirements of this application",
            // This is complicated. We have to check if we are Wayland, xWayland, etc. and there is currently not API in the surface for that.
            .bSupported = false,
            .Items = [this] -> TArray<LPreferenceValue_List::value_type> {
                TArray<LPreferenceValue_List::value_type> Result;
                Result.emplace_back(ListAuto, "Auto");
                algo::for_each(this->GetFrontend().GetPhysicalViewports()
                    , [Result=&Result](LPhysicalViewport const& Viewport)
                {
                    Result->emplace_back(Viewport.Name);
                });
                return Result;
            },)
        MAKE_LIST(DesiredPresentMode,
            .Identifier = "PresentMode",
            .DisplayName = "Present Mode",
            .Tooltip = "How frames are presented",
            .bRequiresImminentRestart = true, // Doesnt have to be. We can just rebuild the swapchains?
            .Items = [this] ->TArray<LPreferenceValue_List::value_type> {

                auto AvailableModes{this->GetFrontend().Vk_GetAvailablePresentModes()};

                TArray<LPreferenceValue_List::value_type> Result;
                Result.reserve(6);
                Result.emplace_back(serde::to_string(rhi::present_mode::Auto));

                Result.emplace_back(serde::to_string(rhi::present_mode::Immediate)
                    , serde::to_string(rhi::present_mode::Immediate)
                    , algo::contains(AvailableModes, rhi::present_mode::Immediate)
                    );
                Result.emplace_back(serde::to_string(rhi::present_mode::Mailbox)
                    , serde::to_string(rhi::present_mode::Mailbox)
                    , algo::contains(AvailableModes, rhi::present_mode::Mailbox)
                    );
                Result.emplace_back(serde::to_string(rhi::present_mode::Fifo)
                    , serde::to_string(rhi::present_mode::Fifo)
                    , algo::contains(AvailableModes, rhi::present_mode::Fifo)
                    );
                Result.emplace_back(serde::to_string(rhi::present_mode::FifoRelaxed)
                    , serde::to_string(rhi::present_mode::FifoRelaxed)
                    , algo::contains(AvailableModes, rhi::present_mode::FifoRelaxed)
                    );
                Result.emplace_back(serde::to_string(rhi::present_mode::FifoLatestReady)
                    , serde::to_string(rhi::present_mode::FifoLatestReady)
                    , algo::contains(AvailableModes, rhi::present_mode::FifoLatestReady)
                    );

                return Result;
            },)
        MAKE_SCALAR_MINMAX(MaxTps,
            .Identifier = "TpsLimit",
            .DisplayName = "Tps Limit",
            .Tooltip = "The upper limit of tps",
            .Step = 1.0,
            .Minimum = 1.0,
            .Maximum = 240.0,
            )

        {
            MAKE_SUBCOLLECTION(
                .Identifier = "LoadBalancing",
                .DisplayName = "Load Balancing",
                .Tooltip =
                    "If the app is minimized, not focused, or not directly interacted with allow this app to enter a sleeping mode to free up CPU/GPU load. "
                    "Memory orphaning is not supported"
                )
            MAKE_BOOLEAN(AllowLoadBalancing,
                .Identifier = "AllowLoadBalancing",
                .DisplayName = "Allow Load Balancing",
                .Tooltip = "Allow load balancing",
                )
            MAKE_SCALAR_MINMAX(LoadBalancingProcessThreshold,
                .Identifier = "LoadBalancingProcessThreshold",
                .DisplayName = "Load Balancing Process Threshold",
                .Tooltip = "The threshold of the load balancing reenter process",
                .Step = 0.01,
                .Minimum = 0.3,
                .Maximum = 5.0,
                )
        }

        {
            MAKE_SUBCOLLECTION(
                .Identifier = "Screenshots",
                .DisplayName = "Screenshots",
                .Tooltip = algo::sprintf(
                    "Taken with the screenshot key (see the controls) of everything that the window shows. Saved as PNG to [{}].",
                    finder::screenshots_dir().string()
                    ),
                )
            MAKE_TEXT(ScreenshotNamePattern,
                .Identifier = "ScreenshotNamePattern",
                .DisplayName = "Name Pattern",
                .Tooltip =
                    "The file name, without the extension. %Y, %m, %d, %H, %M and %S become the year, month, day, hour, "
                    "minute and second. If the file exists, _1, _2, ... is appended.",
                )
            MAKE_LIST(ScreenshotMaxHeight,
                .Identifier = "ScreenshotMaxResolution",
                .DisplayName = "Max Resolution",
                .Tooltip =
                    "The largest height in pixels. Taller windows are scaled down, keeping their aspect ratio, so 1080p "
                    "makes a 3840x2160 window 1920x1080. Smaller windows are saved as they are.",
                .Items = [this] -> TArray<LPreferenceValue_List::value_type> {
                    TArray<LPreferenceValue_List::value_type> Result;
                    Result.emplace_back(serde::to_string(0u), "None");
                    for (const u32 Height: {720u, 1080u, 1440u, 2160u})
                    {
                        Result.emplace_back(serde::to_string(Height), Height == 2160u ? "2160p (4K)" : algo::sprintf("{}p", Height));
                    }
                    /* A height that was set in the config file. */
                    if (LString Current{serde::to_string(*this->ScreenshotMaxHeight)};
                        !algo::contains(Result, Current, &LInputListItem::Identifier))
                    {
                        Result.emplace_back(Current, algo::sprintf("{}p", *this->ScreenshotMaxHeight));
                    }
                    return Result;
                },)
        }
    }

    {
        Controls::InstallControlsCollection(this->TopLevelCollections.emplace_back(LPreferenceCollection::CreateInfo{
            .Identifier = "Controls",
            .DisplayName = "Controls",
            .Tooltip = "The keys of all user input contexts",
            }));
    }

    {
        auto& Collection{this->TopLevelCollections.emplace_back(LPreferenceCollection::CreateInfo{"UserInterface", "User Interface"})};
        MAKE_SCALAR_MINMAX(InterfaceGamma,
            .Identifier = "InterfaceGamma",
            .DisplayName = "Interface Gamma",
            .Tooltip = "Gamma for the interface",
            .Step = 0.01,
            .Minimum = 0.0,
            .Maximum = 10.0,
            )
        MAKE_SCALAR_MINMAX(MouseWheelScrollSpeed,
            .Identifier = "MouseWheelScrollSpeed",
            .DisplayName = "Mouse Wheel Scroll Speed",
            .Tooltip = "Scroll speed for the mouse. Detached from the underling platform preferences.",
            .Step = 0.1,
            .Minimum = 0.1,
            .Maximum = 100.0,
            )
    }

    {
        auto& EditorCollection{this->TopLevelCollections.emplace_back(LPreferenceCollection::CreateInfo{"Editor", "Editor"})};
    }

    {
        auto& DeveloperCollection{this->TopLevelCollections.emplace_back(LPreferenceCollection::CreateInfo{"Developer", "Developer"})};
    }

#undef MAKE_SUBCOLLECTION
#undef MAKE_BOOLEAN
#undef MAKE_SCALAR
#undef MAKE_SCALAR_MINMAX
#undef MAKE_LIST
#undef MAKE_TEXT
}
