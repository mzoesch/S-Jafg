// Copyright mzoesch. All rights reserved.

#include "User/UserPreferences.h"
#include "Framework/Frontend.h"
#include "User/PreferenceCollection.h"
#include "User/PreferenceValue.h"
#include "Widgets/Input_Slider.h"
#include "Widgets/Input_List.h"
#include "Widgets/Input_Boolean.h"
#include "Serialization/StringStatements.h"
#include "User/PreferenceCollection.h"

namespace Jafg
{

constexpr LNodeSize2 MinSize{99_spt, 0};

static LFactoryNode CreateCheckmark(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue& Preference)
{
    return NewNode(Viewport).Class<WInput_Boolean>()
        .Value(serde::from_string<bool>(Preference.GetValue()))
        .OnValueChanged([Changes=&Changes,Preference=&Preference](WInput_Boolean& Self)
        {
            if (auto It{algo::find(*Changes, Preference, [](LPreferenceValueChangeRequest const& Change){ return &Change.Preference; })};
                It == Changes->end())
            {
                Changes->emplace_back(*Preference, serde::to_string(Self.GetValue()));
            }
            else
            {
                It->Value = serde::to_string(Self.GetValue());
            }
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
            if (auto It{algo::find(*Changes, Preference, [](LPreferenceValueChangeRequest const& Change){ return &Change.Preference; })};
                It == Changes->end())
            {
                Changes->emplace_back(*Preference, serde::to_string(Value));
            }
            else
            {
                It->Value = serde::to_string(Value);
            }
        });
}

static LFactoryNode CreateList(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue_List& Preference)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    LString Value{Preference.GetValue()};
    return NewNode(Viewport).Class<WInput_List>()
        .MinDesiredSize(MinSize)
        .Tint(*Prefs.InputColor)
        .OutlineThickness(1)
        .OutlineTint({0x8F})
        .Padding({5_spt, 5})
        .TextTint(Colors::White)
        .Selected(Value.empty() ? LString{"<None>"} : std::move(Value))
        .Items(Preference.GetItems())
        .OnValueChanged([Changes=&Changes, Preference=&Preference](LInputListItem const& Value)
        {
            if (auto It{algo::find(*Changes, Preference, [](LPreferenceValueChangeRequest const& Change){ return &Change.Preference; })};
                It == Changes->end())
            {
                Changes->emplace_back(*Preference, Value.Identifier);
            }
            else
            {
                It->Value = Value.Identifier;
            }
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
        .Setter = [this](LString const& Value) { JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); }, \
        .CreateEditNode = CreateList, \
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
                TArray<LPreferenceValue_List::value_type> Devices;
                algo::for_each(this->GetFrontend().Vk_GetAvailablePhysicalDevices(), [Devices=&Devices](LFrontend::LRankedPhysicalDevice const& Device)
                {
                    Devices->emplace_back(Device.PhysicalDevice.getProperties().deviceName);
                });
                return Devices;
            },)
        MAKE_LIST(PreferredPhysicalViewport,
            .Identifier = "PhysicalViewport",
            .DisplayName = "Physical Viewport",
            .Tooltip = "Physical viewports fulfilling the requirements of this application",
            // This is complicated. We have to check if we are Wayland, xWayland, etc. and there is currently not API in the surface for that.
            .bSupported = false,
            .Items = [this] -> TArray<LPreferenceValue_List::value_type> {
                TArray<LPreferenceValue_List::value_type> Viewports;
                algo::for_each(this->GetFrontend().GetPhysicalViewports(), [Viewports=&Viewports](LPhysicalViewport const& Viewport)
                {
                    Viewports->emplace_back(Viewport.Name);
                });
                return Viewports;
            },)
        MAKE_LIST(PreferredPhysicalViewport,
            .Identifier = "PresentMode",
            .DisplayName = "Present Mode",
            .Tooltip = "How frames are presented",
            .bRequiresImminentRestart = true, // Doesnt have to be. We can just rebuild the swapchains?
            .Items = [this] ->TArray<LPreferenceValue_List::value_type> {

                auto AvailableModes{this->GetFrontend().Vk_GetAvailablePresentModes()};

                TArray<LPreferenceValue_List::value_type> PresentModes;
                PresentModes.reserve(5);

                PresentModes.emplace_back(serde::to_string(rhi::present_mode::Immediate)
                    , serde::to_string(rhi::present_mode::Immediate)
                    , algo::contains(AvailableModes, rhi::present_mode::Immediate)
                    );
                PresentModes.emplace_back(serde::to_string(rhi::present_mode::Mailbox)
                    , serde::to_string(rhi::present_mode::Mailbox)
                    , algo::contains(AvailableModes, rhi::present_mode::Mailbox)
                    );
                PresentModes.emplace_back(serde::to_string(rhi::present_mode::Fifo)
                    , serde::to_string(rhi::present_mode::Fifo)
                    , algo::contains(AvailableModes, rhi::present_mode::Fifo)
                    );
                PresentModes.emplace_back(serde::to_string(rhi::present_mode::FifoRelaxed)
                    , serde::to_string(rhi::present_mode::FifoRelaxed)
                    , algo::contains(AvailableModes, rhi::present_mode::FifoRelaxed)
                    );
                PresentModes.emplace_back(serde::to_string(rhi::present_mode::FifoLatestReady)
                    , serde::to_string(rhi::present_mode::FifoLatestReady)
                    , algo::contains(AvailableModes, rhi::present_mode::FifoLatestReady)
                    );

                return PresentModes;
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
    }

    {
        auto& ControlsCollection{this->TopLevelCollections.emplace_back(LPreferenceCollection::CreateInfo{"Controls", "Controls"})};
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
}
