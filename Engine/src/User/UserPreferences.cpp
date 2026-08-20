// Copyright mzoesch. All rights reserved.

#include "User/UserPreferences.h"

#include "Framework/FrontendVk.h"
#include "User/PreferenceCollection.h"
#include "User/PreferenceValue.h"
#include "Widgets/Input_Slider.h"
#include "Widgets/Input_List.h"
#include "Serialization/StringStatements.h"

namespace Jafg
{

constexpr LNodeSize2 MinSize{99_spt, 0};

LFactoryNode CreateSlider(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes, LPreferenceValue_Scalar& Preference)
{
    return NewNode(Viewport).Class<WInput_Slider>()
        .MinDesiredSize(MinSize)
        .Value(serde::from_string<f64>(Preference.GetValue()))
        .Step(Preference.GetStep())
        .Minimum(Preference.HasMinimum() ? Preference.GetMinimum() : std::numeric_limits<f64>::lowest())
        .Maximum(Preference.HasMaximum() ? Preference.GetMaximum() : std::numeric_limits<f64>::max())
        .OnValueChanged([Changes=&Changes,Preference=&Preference](f64 Value)
        {
            auto It{algo::find(*Changes, static_cast<LPreferenceValue const*>(Preference), [](LPreferenceValueChangeRequest const& Change)
            {
                return &Change.Preference;
            })};
            if (It == Changes->end())
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
            auto It{algo::find(*Changes, static_cast<LPreferenceValue const*>(Preference), [](LPreferenceValueChangeRequest const& Change)
            {
                return &Change.Preference;
            })};
            if (It == Changes->end())
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

#define MAKE_SCALAR_MINMAX(Collection, Id, Display, Preference, ValueStep, Min, Max, EditorNode) \
    Collection.Children.emplace_back(std::make_unique<LPreferenceValue_Scalar>(LPreferenceValue_ScalarCreateInfo{ \
        .Identifier = Id, \
        .DisplayName = Display, \
        .Step = ValueStep, \
        .Minimum = Min, \
        .Maximum = Max, \
        .Getter = [this]{ return JUserPreferences:: JAFG_JOIN_OUTER_TWO(_GetField_, Preference) (*this); }, \
        .Setter = [this](LString const& Value) { JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); }, \
        .CreateEditNode = EditorNode, \
        }));
#define MAKE_SCALAR(Collection, Id, Display, Preference, ValueStep, EditorNode) \
    MAKE_SCALAR_MINMAX( Collection, Id, Display, Preference, ValueStep, this-> Preference .GetMin(), this-> Preference .GetMax(), EditorNode)
#define MAKE_LIST(Collection, Id, Display, Preference, AllowedItems, EditorNode) \
    Collection.Children.emplace_back(std::make_unique<LPreferenceValue_List>(LPreferenceValue_ListCreateInfo{ \
        .Identifier = Id, \
        .DisplayName = Display, \
        .Items = AllowedItems, \
        .Getter = [this]{ return JUserPreferences:: JAFG_JOIN_OUTER_TWO(_GetField_, Preference) (*this); }, \
        .Setter = [this](LString const& Value) { JUserPreferences::JAFG_JOIN_OUTER_TWO(_SetField_, Preference)(this, Value); }, \
        .CreateEditNode = EditorNode, \
        }));

    auto& AudioCollection{this->TopLevelCollections.emplace_back("Audio", "Audio")};
    MAKE_SCALAR(AudioCollection, "MasterVolume", "Master Volume", MasterVolume, 0.01f, CreateSlider)
    MAKE_SCALAR(AudioCollection, "MusicVolume", "Music Volume", MusicVolume, 0.01f, CreateSlider)
    MAKE_SCALAR(AudioCollection, "MiscVolume", "Misc Volume", MiscVolume, 0.01f, CreateSlider)
    MAKE_SCALAR(AudioCollection, "VoiceVolume", "Voice Volume", VoiceVolume, 0.01f, CreateSlider)

    auto& VideoCollection{this->TopLevelCollections.emplace_back("Video", "Video")};
    MAKE_LIST(VideoCollection, "PhysicalDevice", "Physical Device", PreferredPhysicalDevice, [this]
    {
        TArray<LString> Devices;
        algo::for_each(this->GetFrontend().Vk_GetAvailablePhysicalDevices(), [Devices=&Devices](LFrontend::LRankedPhysicalDevice const& Device)
        {
            Devices->emplace_back(Device.PhysicalDevice.getProperties().deviceName);
        });
        return Devices;
    }, CreateList)
    MAKE_LIST(VideoCollection, "PhysicalViewport", "Physical Viewport", PreferredMonitor, [this]
    {
        TArray<LString> Viewports;
        algo::for_each(this->GetFrontend().GetPhysicalViewports(), [Viewports=&Viewports](LPhysicalViewport const& Viewport)
        {
            Viewports->emplace_back(Viewport.Name);
        });
        return Viewports;
    }, CreateList)

    auto& ControlsCollection{this->TopLevelCollections.emplace_back("Controls", "Controls")};

    auto& UserInterfaceCollection{this->TopLevelCollections.emplace_back("UserInterface", "User Interface")};
    MAKE_SCALAR_MINMAX(UserInterfaceCollection, "InterfaceGamma", "Interface Gamma", InterfaceGamma, 0.01f, 0.0f, 10.0f, CreateSlider)
    MAKE_SCALAR_MINMAX(UserInterfaceCollection, "MouseWheelScrollSpeed", "Mouse Wheel Scroll Speed", MouseWheelScrollSpeed, 0.1f, 0.1f, 50.0f, CreateSlider)

    auto& EditorCollection{this->TopLevelCollections.emplace_back("Editor", "Editor")};
    auto& DeveloperCollection{this->TopLevelCollections.emplace_back("Developer", "Developer")};

#undef MAKE_SCALAR
#undef MAKE_SCALAR_MINMAX
#undef MAKE_LIST
}
