// Copyright mzoesch. All rights reserved.

#include "User/UserPreferences.h"
#include "Framework/Frontend.h"
#include "User/PreferenceCollection.h"
#include "User/PreferenceValue.h"
#include "Widgets/Input_Slider.h"
#include "Widgets/Input_List.h"
#include "Widgets/Input_Boolean.h"
#include "Serialization/StringStatements.h"

namespace Jafg
{

constexpr LNodeSize2 MinSize{99_spt, 0};

LFactoryNode CreateCheckmark(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue& Preference)
{
    return NewNode(Viewport).Class<WInput_Boolean>()
        .Value(serde::from_string<bool>(Preference.GetValue()))
        .OnValueChanged([Changes=&Changes,Preference=&Preference](bool Value)
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

LFactoryNode CreateSlider(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue_Scalar& Preference)
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

LFactoryNode CreateList(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue_List& Preference)
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
        .OnValueChanged([Changes=&Changes,Preference=&Preference](LString const& Value)
        {
            if (auto It{algo::find(*Changes, Preference, [](LPreferenceValueChangeRequest const& Change){ return &Change.Preference; })};
                It == Changes->end())
            {
                Changes->emplace_back(*Preference, Value);
            }
            else
            {
                It->Value = Value;
            }
        });
}

} /* ~Namespace <Anonymous> */

void Jafg::JUserPreferences::BeginLife()
{
    Super::BeginLife();

#define MAKE_SUBCOLLECTION(SubCollection, Collection, Id, Display, Tt) \
    auto& SubCollection{*static_cast<LPreferenceCollection*>(&*Collection.Children.emplace_back(std::make_unique<LPreferenceCollection>(Id, Display, Tt)))};
#define MAKE_BOOLEAN(Collection, Id, Display, Tt, Preference, EditorNode) \
    Collection.Children.emplace_back(std::make_unique<LPreferenceValue_Boolean>(LPreferenceValue_BooleanCreateInfo{ \
        .Identifier = Id, \
        .DisplayName = Display, \
        .Tooltip = Tt, \
        .Getter = [this]{ return JUserPreferences:: JAFG_JOIN_OUTER_TWO(_GetField_, Preference) (*this); }, \
        .Setter = [this](LString const& Value) { JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); }, \
        .CreateEditNode = EditorNode, \
        }));
#define MAKE_SCALAR_MINMAX(Collection, Id, Display, Tt, Preference, ValueStep, Min, Max, EditorNode) \
    Collection.Children.emplace_back(std::make_unique<LPreferenceValue_Scalar>(LPreferenceValue_ScalarCreateInfo{ \
        .Identifier = Id, \
        .DisplayName = Display, \
        .Tooltip = Tt, \
        .Step = ValueStep, \
        .Minimum = Min, \
        .Maximum = Max, \
        .Getter = [this]{ return JUserPreferences:: JAFG_JOIN_OUTER_TWO(_GetField_, Preference) (*this); }, \
        .Setter = [this](LString const& Value) { JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); }, \
        .CreateEditNode = EditorNode, \
        }));
#define MAKE_SCALAR(Collection, Id, Display, Tt, Preference, ValueStep, EditorNode) \
    MAKE_SCALAR_MINMAX( Collection, Id, Display, Tt, Preference, ValueStep, this-> Preference .GetMin(), this-> Preference .GetMax(), EditorNode)
#define MAKE_LIST(Collection, Id, Display, Tt, Preference, AllowedItems, EditorNode) \
    Collection.Children.emplace_back(std::make_unique<LPreferenceValue_List>(LPreferenceValue_ListCreateInfo{ \
        .Identifier = Id, \
        .DisplayName = Display, \
        .Tooltip = Tt, \
        .Items = AllowedItems, \
        .Getter = [this]{ return JUserPreferences:: JAFG_JOIN_OUTER_TWO(_GetField_, Preference) (*this); }, \
        .Setter = [this](LString const& Value) { JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); }, \
        .CreateEditNode = EditorNode, \
        }));

    {
        auto& AudioCollection{this->TopLevelCollections.emplace_back("Audio", "Audio")};
        MAKE_SCALAR(AudioCollection, "MasterVolume", "Master Volume", "Volume of the application", MasterVolume, 0.01f, CreateSlider)
        MAKE_SCALAR(AudioCollection, "MusicVolume", "Music Volume", "Volume of music", MusicVolume, 0.01f, CreateSlider)
        MAKE_SCALAR(AudioCollection, "MiscVolume", "Misc Volume", "Volume of miscellaneous stuff", MiscVolume, 0.01f, CreateSlider)
        MAKE_SCALAR(AudioCollection, "VoiceVolume", "Voice Volume", "Volume of voice", VoiceVolume, 0.01f, CreateSlider)
    }

    {
        auto& VideoCollection{this->TopLevelCollections.emplace_back("Video", "Video")};
        MAKE_LIST(VideoCollection, "PhysicalDevice", "Physical Device", "Physical devices fulfilling the requirements of this application", PreferredPhysicalDevice, [this]
        {
            TArray<LString> Devices;
            algo::for_each(this->GetFrontend().Vk_GetAvailablePhysicalDevices(), [Devices=&Devices](LFrontend::LRankedPhysicalDevice const& Device)
            {
                Devices->emplace_back(Device.PhysicalDevice.getProperties().deviceName);
            });
            return Devices;
        }, CreateList)
        MAKE_LIST(VideoCollection, "PhysicalViewport", "Physical Viewport", "Physical viewports fulfilling the requirements of this application", PreferredMonitor, [this]
        {
            TArray<LString> Viewports;
            algo::for_each(this->GetFrontend().GetPhysicalViewports(), [Viewports=&Viewports](LPhysicalViewport const& Viewport)
            {
                Viewports->emplace_back(Viewport.Name);
            });
            return Viewports;
        }, CreateList)
        MAKE_LIST(VideoCollection, "PresentMode", "Present Mode", "How frames are presented", DesiredPresentMode, []
        {
            TArray<LString> PresentModes; PresentModes.reserve(5);
            PresentModes.emplace_back(serde::to_string(rhi::present_mode::Immediate));
            PresentModes.emplace_back(serde::to_string(rhi::present_mode::Mailbox));
            PresentModes.emplace_back(serde::to_string(rhi::present_mode::Fifo));
            PresentModes.emplace_back(serde::to_string(rhi::present_mode::FifoRelaxed));
            PresentModes.emplace_back(serde::to_string(rhi::present_mode::FifoLatestReady));
            return PresentModes;
        }, CreateList) // TODO: flag to rebuild swapchain? also make each item of a list have a unsupported/disabled flag
        MAKE_SCALAR_MINMAX(VideoCollection, "TpsLimit", "Tps limit", "The upper limit of tps", MaxTps, 1.0, 1.0, 240.0, CreateSlider)
        {
            MAKE_SUBCOLLECTION(SleepCollection, VideoCollection, "LoadBalancing", "Load Balancing",
                "If the app is minimized, not focused, or not directly interacted with allow this app to enter a sleeping mode to free up CPU/GPU load. "
                "Memory orphaning is not supported")
            MAKE_BOOLEAN(SleepCollection, "AllowLoadBalancing", "Allow Load Balancing", "Allow load balancing", AllowLoadBalancing, CreateCheckmark)
            MAKE_SCALAR_MINMAX(SleepCollection, "LoadBalancingProcessThreshold", "Load Balancing Process Threshold", "The threshold of the load balancing reenter process", LoadBalancingProcessThreshold, 0.01, 0.3, 5.0, CreateSlider)
        }
    }

    {
        auto& ControlsCollection{this->TopLevelCollections.emplace_back("Controls", "Controls")};
    }

    {
        auto& UserInterfaceCollection{this->TopLevelCollections.emplace_back("UserInterface", "User Interface")};
        MAKE_SCALAR_MINMAX(UserInterfaceCollection
            , "InterfaceGamma", "Interface Gamma", "Gamma for the interface"
            , InterfaceGamma, 0.01f, 0.0f, 10.0f, CreateSlider)
        MAKE_SCALAR_MINMAX(UserInterfaceCollection
            , "MouseWheelScrollSpeed", "Mouse Wheel Scroll Speed", "Scroll speed for the mouse. Detached from the underling platform preferences."
            , MouseWheelScrollSpeed, 0.1f, 0.1f, 100.0f, CreateSlider)
    }

    {
        auto& EditorCollection{this->TopLevelCollections.emplace_back("Editor", "Editor")};
    }

    {
        auto& DeveloperCollection{this->TopLevelCollections.emplace_back("Developer", "Developer")};
    }

#undef MAKE_SUBCOLLECTION
#undef MAKE_BOOLEAN
#undef MAKE_SCALAR_MINMAX
#undef MAKE_SCALAR
#undef MAKE_LIST
}
